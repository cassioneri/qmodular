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
 * @brief Benchmark algorithms for a given function.
 */

#include <cstdint>

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

// Value of divisor.
constexpr uint_t d = 14;

// The 1st argument (dividend) of f is a random variable uniformly distributed
// in [0, bound1].
constexpr uint_t bound1 = 1000000;

// Value of 2nd argument of f (2nd dividend of are_equivalent or remainder of
// other functions). It can be either a fixed positive constant or -1. The
// latter is a special value indicating that n2 is a runtime variable.
constexpr uint_t n2 = 3;

// When n2 is a runtime variable, it is a uniformly distributed random variable
// in [0, bound2].
constexpr uint_t bound2 = d - 1;

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

} // namespace qmodular

int
main(int argc, char* argv[]) {

  using namespace qmodular;
  using namespace qmodular::benchmark;

  auto const points = data<uint_t>(n_points, bound1, bound2);

  if (!is_quick_bench)
    register_no_op<uint_t, n2>(points);

  register_algos<uint_t, d, n2, bound1, bound2, f>(algos(), points);

  if (is_quick_bench)
    register_no_op<uint_t, n2>(points);

  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();
}
