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
 * @file inspection.cpp
 *
 * @brief Forces instantiations of algorithms for inspection of generated code.
 */

#include <cstdint>

#include "built_in.hpp"
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

// Value of divisor.
constexpr uint_t d = 14;

// Value of objective function's 2nd argument (2nd dividend of are_equivalent or
// remainder of other functions). It can be either a fixed positive constant or
// -1. The latter is a special value indicating that n2 is a runtime variable.
constexpr uint_t n2 = 3;

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

// -----------------------------------------------------------------------------

/**
 * @brief Instantiates objective function for a given algorithm.
 *
 * @tparam A    Algorithm to be used.
 *
 * @tparam n    1st argument.
 * @tparam m    2nd argument.
 */
template <template <class> class A>
bool
instantiate_algo(uint_t n, uint_t m) noexcept {

  if constexpr (n2 != uint_t(-1) && f != function::are_equivalent)
    m = n2;

  auto constexpr algo = callable<A<uint_t>, f>(d);
  return algo(n, m);
}

/**
 * @brief Instantiates objective function for given algorithms.
 *
 * @tparam A    First algorithm to be used.
 * @tparam As   Other algorithms to be used.
 */
template <template <class> class A, template <class> class... As>
std::uintptr_t
instantiate_algos(algo_list<A, As...>) noexcept {

  std::uintptr_t p = 0;

  if constexpr (does_implement<A<uint_t>, f>) {

    using algo       = callable<A<uint_t>, f>;
    auto constexpr a = algo(d);

    if constexpr (n2 == uint_t(-1) || n2 <= a.max_2nd())
      p = reinterpret_cast<std::uintptr_t>(&instantiate_algo<A>);
  }

  if constexpr (sizeof...(As) > 0)
    p += reinterpret_cast<std::uintptr_t>(instantiate_algos(algo_list<As...>()));

  return p;
}

template std::uintptr_t instantiate_algos(algos);

} // namespace qmodular
