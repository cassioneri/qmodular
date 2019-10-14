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

#include "built_in.hpp"
#include "math.hpp"
#include "meta.hpp"
#include "minverse.hpp"
#include "mshift.hpp"
#include "new_algo.hpp"
#include "time.hpp"

using namespace qmodular;

//------------------------------------------------------------------------------
// Configuration
//------------------------------------------------------------------------------

struct Config {

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

  // The number of n_divisors in the range.
  static constexpr uint_t n_divisors = 1000;

  // The objective function is called on n_points data points.
  static constexpr unsigned n_points = 65536;

  // Objective function's 1st argument (dividend) is a random variable uniformly
  // distributed in [0, bound].
  static constexpr uint_t bound = 1000000;

  // Objective function's 2nd argument can be either a fixed positive constant
  // or -1. The latter is a special value indicating the 2nd argument is a
  // runtime variable.
  static constexpr uint_t n2 = -1;

}; // struct Config

int
main(int argc, char* argv[]) {

  using namespace ::qmodular::measure;

  TimeRegistrar<typename Config::uint_t> registrar;

  register_n<Config>(registrar);

  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();
}
