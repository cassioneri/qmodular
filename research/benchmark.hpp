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

#pragma once

#include <random>
#include <string>
#include <type_traits>
#include <vector>

namespace qmodular {

constexpr auto is_quick_bench =
  #ifdef BENCHMARK // benchmark.h has been included, supposedly by quick-bench.
    true;
  #else
    false;
  #endif
}

#include <benchmark/benchmark.h>

#include "meta.hpp"

// Prevents quick-bench from duplicating of main.
#undef  BENCHMARK_MAIN
#define BENCHMARK_MAIN()

// Prevents registration of quick-bench's implementation of Noop.
#undef  BENCHMARK
#define BENCHMARK(x)

namespace qmodular::benchmark {

/**
 * @brief Container of test cases.
 *
 * Each test case is given by a pair of numbers (n1, n2). Each pair is passed to
 * the function being measured.
 *
 * @tparam U    Type of numbers.
 */
template <class U>
struct data {

  struct args {
    U n1;
    U n2;
  };

  /**
   * @brief Constructor.
   *
   * @param size    Number of test cases.
   * @param bound1  1st argument is drawn in [0, bound1].
   * @param bound2  2nd argument is drawn in [0, bound2].
   */
  data(std::size_t size, U bound1, U bound2) noexcept {

    data_.resize(size);

    std::mt19937 rng;
    rng.seed(std::random_device()());

    std::uniform_int_distribution<U> d1(0, bound1);
    std::uniform_int_distribution<U> d2(0, bound2);

    for (std::size_t i = 0; i < size; ++i) {
      data_[i].n1 = d1(rng);
      data_[i].n2 = d2(rng);
    }
  }

  /**
   * @brief Accesses the i-th test case.
   *
   * @param i     Index.
   */
  const args& operator [](std::size_t i) const noexcept {
    return data_[i];
  }

  /**
   * @brief Returns the number of test cases.
   */
  std::size_t size() const noexcept {
    return data_.size();
  }

private:

  std::vector<args> data_;
};

namespace detail {

/**
 * @brief Runs a callable object for time measurement.
 *
 * The callable object is constexpr-built from a given divisor d and expects two
 * input arguments. The 1st one is taken from a given object pts whereas the 2nd
 * argument might be fixed to n2 or also taken from pts. More precisely, when
 * n2 != -1 this value is used* as the 2nd argument. Otherwise, it is taken from
 * pts.
 *
 * @tparam U    Type of d and n2.
 * @tparam d    Divisor.
 * @tparam n2   If this value is not U(-1) then it is used ad the 2nd argument
 *              passed to the measured function. Otherwise, the 2nd argument
 *              is taken from pts (below).
 * @tparam A    Type of callable object.
 * @param  s    The benchmark state. (See Google's benchmark for more details.)
 * @param  pts  Contains the values used as 1st and (possibly) 2nd arguments of
 *              callable object.
 */
template <class U, U d, U n2, class A>
void
// For fairer results, we need to turn "move-loop-invariants" optimisation off.
// Indeed, some functions profiled here contain a multiplication. To perform the
// calculation, the generated assembly loads either one or both multiplicands
// into registers and, since this load is part of the algorithm, we want the
// measurement to take this cost into account. However, one of the multiplicands
// is a loop invariant since it only depends on the divisor. In full
// optimisation mode the compiler moves the load out of the two loops below.
// This would completely remove the cost of the load from the measurement.
// Unfortunately, clang 8.0.0 does not provide the same functionality.
__attribute__((optimize("-fno-move-loop-invariants")))
run(::benchmark::State& s, data<U> const& points) {

  auto constexpr a = A(d);
  auto const     n = points.size();

  for (auto _ : s) {
    for (std::size_t i = 0; i < n; ++i) {

      // Variable point must not be const otherwise it might be optimised away
      // defeating the purpose of DoNotOptimize which we do need here. Indeed,
      // we want all measurements to consider the cost of loading points from
      // memory even those that are not going to be used. In that way, the
      // instructions inside this loop that differ from one algorithm to another
      // will be exclusively those in the algorithms themselves and not from the
      // scaffolding around measurements.
      auto point = points[i];
      ::benchmark::DoNotOptimize(point);

      if constexpr(!std::is_same_v<void, decltype(a(point.n1, point.n2))>) {
        if constexpr (n2 != U(-1)) {
          // Same comment as above.
          auto x = a(point.n1, n2);
          ::benchmark::DoNotOptimize(x);
        }
        else {
          // Same comment as above.
          auto x = a(point.n1, point.n2);
          ::benchmark::DoNotOptimize(x);
        }
      }
    }
  }
}

/**
 * @brief A dummy algorithm that does not do anything. It is used to measure the
 * scaffolding code.
 */
template <class U>
struct no_op {
  constexpr no_op(U) noexcept {
  }
  constexpr void
  operator()(U, U) const noexcept {
  }
};

/**
 * @brief Registers a set of algorithms for benchmarking.
 *
 * Given a function f and a set of algorithms that (might) implement f, this
 * function registers a corresponding instantiation of run (above) for
 * benchmarking. Most of this function's arguments are forwarded to run.
 *
 * When the algorithm does not implement f, it is disregarded and not
 * registered for benchmarking.
 *
 * Finally, the implementations of f given by the algorithm might have
 * preconditions on arguments. Generally, 1st and 2nd arguments must be in some
 * intervals, say, [0, c1] and [0, c2], respectively. However, their values
 * (when not fixed) are taken from argument pts (below) and belong to intervals
 * [0, b1] and [0, b2]. If b1 > c1 or b2 > c2, then it is possible that a value
 * taken from pts does not fulfil the precondition. In such case, the algorithm
 * is also disregarded.
 *
 * @tparam U    Type of d, n2, b1 and b2.
 * @tparam d    Divisor. (See run's doc.)
 * @tparam n2   If this value is not U(-1) then it is used ad the 2nd argument
 *              passed to the measured function. Otherwise, the 2nd argument
 *              is taken from pts (below).
 * @tparam b1   1st argument's upper bound.
 * @tparam b2   2nd argument's upper bound.
 * @tparam f    Function to be measured.
 * @tparam A    1st algorithm.
 * @tparam As   Other algorithms.
 * @param  pts  Contains the array of values used as 1st and (possibly) 2nd
 *              arguments of the measured function.
 */
template <class U, U d, U n2, U b1, U b2, function f, template <class> class A,
  template <class> class... As>
void
register_algos(algo_list<A, As...>, data<U> const& pts) {

  if constexpr (does_implement<A<U>, f>) {

    using algo       = callable<A<U>, f>;
    auto constexpr a = algo(d);

    if (b1 <= a.max_1st() && b2 <= a.max_2nd()) {
      auto const name = std::string(algo_name<A>) + '<' + std::to_string(d) +
        '>';
      ::benchmark::RegisterBenchmark(name.c_str(), detail::run<U, d, n2, algo>,
        pts);
    }
  }

  if constexpr (sizeof...(As) > 0)
    detail::register_algos<U, d, n2, b1, b2, f, As...>(algo_list<As...>(), pts);
}

} // namespace detail

/**
 * @brief Registers an algorithm that does not do anything for benchmarking.
 *
 * Different algorithms provide a different implementation of a given function.
 * Each implementation is called on the same set of arguments and the execution
 * time is measured. Actually, some algorithms might not implement the function
 * and in this case they are simply ignored.
 *
 * @tparam U    Type of n2.
 * @tparam n2   If this value is not U(-1) then it is used ad the 2nd argument
 *              passed to the measured function. Otherwise, the 2nd argument
 *              is taken from pts (below).
 * @param  pts  Contains the array of values used as 1st and (possibly) 2nd
 *              arguments of the measured function.
 */
template <class U, U n2>
void
register_no_op(data<U> const& pts) {
  ::benchmark::RegisterBenchmark("Noop", &detail::run<U, 1, n2,
    detail::no_op<U>>, pts);
}

/**
 * @brief Registers a set of algorithms for benchmarking.
 *
 * Given a function f and a set of algorithms that (might) implement f, this
 * function registers a corresponding instantiation of run (above) for
 * benchmarking. Most of this function's arguments are forwarded to run.
 *
 * For fixed remainders, some functions are constant (either true or false). For
 * instance, has_remainder_less(n, 0) == false for all n whereas
 * has_remainder_greater_equal(n, 0) == true for all n. In such cases, the
 * function and all algorithms that implement it are disregarded and not
 * registered for benchmarking.
 *
 * When the algorithm does not implement f, it is also disregarded.
 *
 * Finally, the implementations of f given by the algorithm might have
 * preconditions on arguments. Generally, 1st and 2nd arguments must be in some
 * intervals, say, [0, c1] and [0, c2], respectively. However, their values
 * (when not fixed) are taken from argument pts (below) and belong to intervals
 * [0, b1] and [0, b2]. If b1 > c1 or b2 > c2, then it is possible that a value
 * taken from pts does not fulfil the precondition. In such case, the algorithm
 * is also disregarded.
 *
 * @tparam U    Type of d, n2, b1 and b2.
 * @tparam d    Divisor. (See run's doc.)
 * @tparam n2   If this value is not U(-1) then it is used ad the 2nd argument
 *              passed to the measured function. Otherwise, the 2nd argument
 *              is taken from pts (below).
 * @tparam b1   1st argument's upper bound.
 * @tparam b2   2nd argument's upper bound.
 * @tparam f    Function to be measured.
 * @tparam A    1st algorithm.
 * @tparam As   Other algorithms.
 * @param  pts  Contains the array of values used as 1st and (possibly) 2nd
 *              arguments of the measured function.
 */
template <class U, U d, U n2, U b1, U b2, function f, template <class> class A,
  template <class> class... As>
void
register_algos(algo_list<A, As...>, data<U> const& pts) {

  auto constexpr f_and_n2_are_compatible = [=]() {

    auto constexpr is_variable = n2 == U(-1);

    if constexpr (f == function::has_remainder)
      return is_variable || (n2 >= 0 && n2 <= d - 1);

    if constexpr (f == function::has_remainder_less)
      return is_variable || (n2 >= 1 && n2 <= d - 1);

    if constexpr (f == function::has_remainder_less_equal)
      return is_variable || (n2 >= 0 && n2 <= d - 2);

    if constexpr (f == function::has_remainder_greater)
      return is_variable || (n2 >= 0 && n2 <= d - 2);

    if constexpr (f == function::has_remainder_greater_equal)
      return is_variable || (n2 >= 1 && n2 <= d - 1);

    if constexpr (f == function::are_equivalent)
      return is_variable;

    return false;
  }();

  if (f_and_n2_are_compatible)
    detail::register_algos<U, d, n2, b1, b2, f, A, As...>(algo_list<A, As...>(),
      pts);
}

} // namespace qmodular::benchmark
