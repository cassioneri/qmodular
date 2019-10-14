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

#include <benchmark/benchmark.h>

#include "measure.hpp"

/**
 * @file time.hpp
 *
 * @brief Time measurement functions and classes. The time measurement is based
 * on Google's benchmark library.
 *
 * [1] https://github.com/google/benchmark
 */

namespace qmodular::measure {

constexpr auto is_quick_bench =
  #ifdef BENCHMARK // benchmark.h has been included, supposedly by quick-bench.
    true;
  #else
    false;
  #endif

// Prevents quick-bench from duplicating of main.
#undef  BENCHMARK_MAIN
#define BENCHMARK_MAIN()

// Prevents registration of quick-bench's implementation of Noop.
#undef  BENCHMARK
#define BENCHMARK(x)

template <class U>
class TimeRegistrar {

public:

  template <U n2, U d, class A>
  void book(const char* name, data<U> const& points) noexcept {
    ::benchmark::RegisterBenchmark(name, &run<n2, d, A>, points);
  }

  /**
   * @brief Registers no_op for time measurement. Useful to asses the
   * measurement overhead.
   *
   * @tparam n2       If n2 != -1 then it is used as the 2nd argument passed to
   *                  the measured function. Otherwise, the 2nd argument is
   *                  taken from points.
   * @param  points   Contains the array of values used as 1st and (possibly)
   *                  2nd arguments (when they are variable) of the measured
   *                  function.
   */
  template <U n2>
  void
  book_no_op(data<U> const& points) {
    book<n2, 1, no_op<U>>(algo_name<no_op>, points);
  }

private:

  /**
   * @brief Runs a callable object for time measurement.
   *
   * The callable object is constexpr-built from a given divisor d and expects two
   * input arguments. The 1st one is taken from a given object pts whereas the 2nd
   * argument might be fixed to n2 or also taken from pts. More precisely, when
   * n2 != -1 this value is used* as the 2nd argument. Otherwise, it is taken from
   * pts.
   *
   * @tparam n2       If n2 != -1 then it is used as the 2nd argument passed to
   *                  the measured function. Otherwise, the 2nd argument is
   *                  taken from points.
   * @tparam d        Divisor.
   * @tparam A        Type of callable object that is measured.
   * @param  s        The benchmark state. (See Google's benchmark for more
   *                  details.)
   * @param  points   Contains the array of values used as 1st and (possibly)
   *                  2nd arguments (when they are variable) of the measured
   *                  function.
   */
  template <U n2, U d, class A>
  static void
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

}; // class TimeRegistrar

} // namespace qmodular::measure
