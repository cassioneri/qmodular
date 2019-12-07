/*******************************************************************************
 *
 * Copyright (C) 2019 Cassio Neri
 *
 * This file is part of qmodular.
 *
 * qmodular is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * qmodular is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * qmodular. If not, see <https://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

/**
 * @file time.cpp
 *
 * @brief Time measurement functions and classes. The time measurement is based
 * on Google's benchmark library.
 *
 * [1] https://github.com/google/benchmark
 */

#include <cstdint>
#include <deque>
#include <string>
#include <random>
#include <vector>

#include "built_in.hpp"
#include "math.hpp"
#include "meta.hpp"
#include "measure.hpp"
#include "minverse.hpp"
#include "mshift.hpp"
#include "new_algo.hpp"

using namespace qmodular;
using namespace qmodular::measure;

//------------------------------------------------------------------------------
// Configuration
//------------------------------------------------------------------------------

struct config {

  // Type of dividends, divisors and remainders.
  using uint_t = std::uint32_t;

  // The objective function.
  static constexpr function f = function::has_remainder;

  // List of algorithms to be used.
  using algos = algo_list<
    built_in::plain
    , built_in::distance
    , minverse::plain
    , mshift::plain
    , mshift::promoted
    , new_algo::plain
  >;

  // Each divisor in a range of integers starting from d is considered.
  static constexpr uint_t d = 14;

  // The number of n_divisors in the range (not used by quick-bench)
  static constexpr uint_t n_divisors = 1;

  // The objective function is called on n_points data points.
  static constexpr unsigned n_points = 65536;

  // Objective function's 1st argument (dividend) is a random variable uniformly
  // distributed in [0, bound].
  static constexpr uint_t bound = 1000000;

  // Objective function's 2nd argument can be either a fixed positive constant
  // or -1. The latter is a special value indicating the 2nd argument is a
  // runtime variable.
  static constexpr uint_t n2 = 3;

}; // struct config

//------------------------------------------------------------------------------

#ifdef BENCHMARK // benchmark.h has been included, supposedly by quick-bench.
  constexpr auto is_quick_bench = true;
#else
  #include <benchmark/benchmark.h>
  constexpr auto is_quick_bench = false;
#endif

// Prevents quick-bench from duplicating of main.
#undef  BENCHMARK_MAIN
#define BENCHMARK_MAIN()

// Prevents registration of quick-bench's implementation of Noop.
#undef  BENCHMARK
#define BENCHMARK(x)

/**
 * @brief Container of benchmark data.
 *
 * Each data point is a pair of arguments (n1, n2) that is passed to the
 * function being benchmarked.
 */
struct data {

  struct args_t {
    config::uint_t n1;
    config::uint_t n2;
  };

  /**
   * Default constructor.
   *
   * @post this->empty() == true.
   */
  data() = default;

  /**
   * @brief Constructor.
   *
   * @param size    Number of data points.
   * @param bounds  1st argument is uniformly drawn in [0, bounds.n1] and
   *                2nd argument is uniformly drawn in [0, bounds.n2].
   */
  data(unsigned size, args_t bounds) noexcept {

    data_.resize(size);

    std::mt19937 rng;
    rng.seed(std::random_device()());

    std::uniform_int_distribution<config::uint_t> d1(0, bounds.n1);
    std::uniform_int_distribution<config::uint_t> d2(0, bounds.n2);

    for (std::size_t i = 0; i < size; ++i) {
      data_[i].n1 = d1(rng);
      data_[i].n2 = d2(rng);
    }
  }

  /**
   * @brief Accesses the i-th data point.
   *
   * @param i     Index.
   */
  const args_t& operator [](std::size_t i) const noexcept {
    return data_[i];
  }

  /**
   * @brief Checks whether the data set is empty.
   */
  bool empty() const noexcept {
    return data_.empty();
  }

private:

  std::vector<args_t> data_;

}; // struct data

/**
 * Registers algorithms for benchmarking, constructing the data points to be
 * used in the benchmarking.
 */
class TimeRegistrar {

public:

  using uint_t = config::uint_t;

  /**
   * Registers an algorithm implementing config::f provided preconditions are
   * certain to hold for every data point used during the benchmarking.
   */
  template <uint_t d, template <class> class A>
  void book() noexcept {

    using          algo = callable<A<uint_t>, config::f>;
    auto constexpr a    = algo(d);

    auto constexpr bound2 = config::f == function::are_equivalent ?
        config::bound : d - 1;

    if (config::bound <= a.max_1st() && bound2 <= a.max_2nd()) {
      add_data(d, bound2);
      auto const label = algo_name<A> + ('<' + std::to_string(d) + '>');
      ::benchmark::RegisterBenchmark(label.c_str(), &run<d, algo>, &data_[d]);
    }
  }

  /**
   * @brief Registers no_op for time measurement. )Useful to asses the
   * measurement overhead.)
   */
  void
  book_no_op() {
    add_data(1, config::bound);
    ::benchmark::RegisterBenchmark(algo_name<no_op>, &run<1, no_op<uint_t>>,
        &data_[1]);
  }

private:

  /**
   * @brief Runs a callable object for time measurement.
   *
   * The callable object is constexpr-built from a given divisor d and expects
   * two input arguments. The 1st one is taken from a given object pts whereas
   * the 2nd argument might be fixed to n2 or also taken from pts. More
   * precisely, when * n2 != -1 this value is used* as the 2nd argument.
   * Otherwise, it is taken from points.
   *
   * @tparam d        Divisor.
   * @tparam A        Type of callable object that is measured.
   * @param  s        The benchmark state. (See Google's benchmark for more
   *                  details.)
   * @param  points   Contains the array of values used as 1st and (possibly)
   *                  2nd arguments (when they are variable) of the measured
   *                  function.
   */
  template <uint_t d, class A>
  static void
  // For fairer results, we need to turn "move-loop-invariants" optimisation
  // off. Indeed, some functions profiled here contain a multiplication. To
  // perform the calculation, the generated assembly loads either one or both
  // multiplicands into registers and, since this load is part of the algorithm,
  // we want the measurement to take this cost into account. However, one of the
  // multiplicands is a loop invariant since it only depends on the divisor. In
  // full optimisation mode the compiler moves the load out of the two loops
  // below. This would completely remove the cost of the load from the
  // measurement.
  __attribute__((optimize("-fno-move-loop-invariants")))
  run(::benchmark::State& s, data const* points) {

    auto constexpr a = A(d);

    for (auto _ : s) {
      for (unsigned i = 0; i < config::n_points; ++i) {

        auto address = &(*points)[i];
        __builtin_prefetch(address);
        auto point = *address;
        ::benchmark::DoNotOptimize(point);

        // Variable n1 must not be const otherwise it might be optimised away
        // defeating the purpose of DoNotOptimize which we do need here. Indeed,
        // we want all measurements to consider the cost of loading points from
        // memory even those that are not going to be used. In that way, the
        // instructions inside this loop that differ from one algorithm to
        // another will be exclusively those in the algorithms themselves and
        //  not from the scaffolding around measurements.
        auto n1 = point.n1;
        ::benchmark::DoNotOptimize(n1);

        if constexpr (config::n2 != uint_t(-1)) {
          if constexpr(!std::is_same_v<void, decltype(a(n1, config::n2))>) {
            auto x = a(n1, config::n2);
            ::benchmark::DoNotOptimize(x);
          }
        }
        else {
          auto n2 = point.n2;
          //::benchmark::DoNotOptimize(n2);
          if constexpr(!std::is_same_v<void, decltype(a(n1, n2))>) {
            auto x = a(n1, n2);
            ::benchmark::DoNotOptimize(x);
          }
        }
      }
    }
  }

  void add_data(uint_t d, uint_t bound2) {
    if (data_.size() < d + 1)
      data_.resize(d + 1);
    if (data_[d].empty())
      data_[d] = data(config::n_points, { config::bound, bound2 });
  }

  std::deque<data> data_;

}; // class TimeRegistrar

int
main(int argc, char* argv[]) {

  TimeRegistrar registrar;

  if constexpr (is_quick_bench) {
    register_1<config>(registrar);
    registrar.book_no_op();
  }
  else {
    registrar.book_no_op();
    register_n<config>(registrar);
  }
  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();
}
