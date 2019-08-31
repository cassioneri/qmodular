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
 * @file built_in.hpp
 *
 * @brief Modular expressions using built-in operator %.
 */

#pragma once

#include "adaptors.hpp"
#include "math.hpp"
#include "meta.hpp"

namespace qmodular {
namespace built_in {
namespace impl {

/**
 * @brief Modular evaluations using built in operator %.
 *
 * @tparam U    Divisor's value type.
 * @pre         std::is_unsigned_v<U>
 */
template <class U>
struct algo {

  using uint_t    = U;
  using divisor_t = U;

  /**
   * @brief Creates the algorithm for a given divisor.
   *
   * @param d   Divisor.
   * @pre       d > 0
   */
  constexpr explicit
  algo(uint_t d) noexcept : d(d) {
  }

  /**
   * @brief Returns the divisor.
   */
  constexpr divisor_t const&
  divisor() const noexcept {
    return d;
  }

  /**
   * @brief Returns the maximum allowed input dividend.
   */
  constexpr uint_t
  max_dividend() const noexcept {
    return math::max<uint_t>;
  }

  /**
   * @brief Returns the maximum allowed input remainder.
   */
  constexpr uint_t
  max_remainder() const noexcept {
    return math::max<uint_t>;
  }

  /**
   * @brief Returns the result of n % divisor() == r.
   *
   * @param n   Dividend.
   * @param r   Remainder.
   */
  constexpr bool
  has_remainder(uint_t n, uint_t r) const noexcept {
    return n % divisor() == r;
  }

  /**
   * @brief Returns the result of n % divisor() < r.
   *
   * @param n   Dividend.
   * @param r   Remainder.
   */
  constexpr bool
  has_remainder_less(uint_t n, uint_t r) const noexcept {
    return n % divisor() < r;
  }

  /**
   * @brief Returns the result of n % divisor() <= r.
   *
   * @param n   Dividend.
   * @param r   Remainder.
   */
  constexpr bool
  has_remainder_less_equal(uint_t n, uint_t r) const noexcept {
    return n % divisor() <= r;
  }

  /**
   * @brief Returns the result of n % divisor() > r.
   *
   * @param n   Dividend.
   * @param r   Remainder.
   */
  constexpr bool
  has_remainder_greater(uint_t n, uint_t r) const noexcept {
    return n % divisor() > r;
  }

  /**
   * @brief Returns the result of n % divisor() >= r.
   *
   * @param n   Dividend.
   * @param r   Remainder.
   */
  constexpr bool
  has_remainder_greater_equal(uint_t n, uint_t r) const noexcept {
    return n % divisor() >= r;
  }

  /**
   * Returns the result of n % divisor() == m % divisor().
   *
   * @param n   1st dividend.
   * @param m   2nd dividend.
   */
  constexpr bool
  are_equivalent(uint_t n, uint_t m) const noexcept {
    return n % divisor() == m % divisor();
  }

private:

  divisor_t d;

}; // class algo

} // namespace impl

template <class U>
using plain = adaptors::plain<impl::algo<U>>;

template <class U>
using distance = adaptors::equivalence<impl::algo<U>>;

} // namespace built_in

template <>
constexpr const char* algo_name<built_in::plain> = "built_in";

template <>
constexpr const char* algo_name<built_in::distance> = "built_in_distance";

} // namespace qmodular
