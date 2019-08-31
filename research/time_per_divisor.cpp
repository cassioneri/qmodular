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
 * @file time_per_divisor.cpp
 *
 * @brief Benchmark algorithms for a given function and range of divisors.
 */

#include <cstdint>
#include <utility>

#include "benchmark.hpp"
#include "built_in.hpp"
#include "math.hpp"
#include "meta.hpp"
#include "minverse.hpp"
#include "mshift.hpp"
#include "new_algo.hpp"

namespace qmodular {

//------------------------------------------------------------------------------
// Configuration
//------------------------------------------------------------------------------

// Type of dividends, divisors and remainders.
using uint_t = std::uint32_t;

// The objective function.
constexpr function f = function::has_remainder;

// Number of data points.
constexpr std::size_t n_points = 65536;

// Test are run for all divisors in [first, last].
constexpr uint_t first = 1;
constexpr uint_t last  = 1000;

// The 1st argument (dividend) of f is a random variable uniformly distributed
// in [0, bound1].
constexpr uint_t bound1 = 1000000;

// Value of 2nd argument of f (2nd dividend of are_equivalent or remainder of
// other functions). It can be either a fixed positive constant or -1. The
// latter is a special value indicating that n2 is a runtime variable.
constexpr uint_t n2 = -1;

// List of algorithms to be used. (Some are disregarded when their preconditions
// do not hold.)
using algos = algo_list<
  built_in::plain
  , built_in::distance
  , minverse::plain
  , mshift::plain
  , mshift::promoted
  , new_algo::plain
>;

//------------------------------------------------------------------------------

/**
 * @brief Registers benchmark function for given divisor.
 *
 * @tparam d    Divisor.
 */
template <uint_t d>
void
register_algos() noexcept {

  using namespace qmodular::benchmark;

  auto constexpr bound2 = f == function::are_equivalent ? bound1 : d - 1;
  auto const points     = data<uint_t>(n_points, bound1, bound2);

  if constexpr (d == first)
    register_no_op<uint_t, n2>(points);

  register_algos<uint_t, d, n2, bound1, bound2, f>(algos(), points);
}

/**
 * @brief Registers benchmark function for divisors in {d + x1, ..., d + xN].
 *
 * @tparam d    See above.
 * @tparam x    1st element of  {x1, ... xN}.
 * @tparam xs   Other elements of {x1, ...., xN}.
 */
template <uint_t d, uint_t x, uint_t... xs>
void
register_divisors() noexcept {

  register_algos<d + x>();

  if constexpr (sizeof...(xs) > 0)
    register_divisors<d, xs...>();
}

/**
 * @brief Registers benchmark function for divisors in {d + x1, ..., d + xN].
 *
 * @tparam d    See above.
 * @tparam xs   Set {x1, ...., xN} as described above.
 */
template <uint_t d, uint_t... xs>
void
register_divisors(std::integer_sequence<uint_t, xs...>) noexcept {
  register_divisors<d, xs...>();
}

/**
 * @brief Registers benchmark function for divisors in [d1, d2].
 *
 * Compiler limits are reached if d2 - d1 is large enough.
 *
 * @tparam d1   First divisor in the range.
 * @tparam d2   Last  divisor in the range.
 */
template <uint_t d, uint_t d2>
void
register_small_range() noexcept {
  auto const sequence = std::make_integer_sequence<uint_t, d2 - d + 1>();
  register_divisors<d>(sequence);
}

/**
 * @brief Registers benchmark function for divisors in [d1, d2].
 *
 * Compiler limits are reached if d2 - d1 is large enough. This functions tries
 * to mitigate this issue by breaking the registration into intervals with at
 * most 500 divisors.
 *
 * @tparam d1   First divisor in the range.
 * @tparam d2   Last  divisor in the range.
 */
template <uint_t d, uint_t d2>
void
register_range() noexcept {

  if constexpr (d2 - d < 500)
    register_small_range<d, d2>();

  else {
    register_small_range<d, d + 499>();
    register_small_range<d + 500, d2>();
  }
}

} // namespace qmodular

int
main(int argc, char* argv[]) {

  using namespace qmodular;

  register_range<first, last>();

  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();
}
