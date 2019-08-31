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
 * @file mshift.hpp
 *
 * @brief Modular expressions using remainder by multiply and shift algorithm.
 *
 * Essentially, these are the algorithms presented in [Warren] section 10-20.
 * However, the implementation here do not compute remainders but stop when
 * there is enough information for remainder comparison.
 *
 * [Warren] Warren, H.S., "Hacker's delight." Addison-Wesley, 2013.
 */

#pragma once

#include "adaptors.hpp"
#include "math.hpp"
#include "meta.hpp"

namespace qmodular {
namespace mshift {
namespace impl {

/**
 * @brief Divisor information.
 *
 * @tparam U    Divisor's value type.
 * @pre         std::is_unsigned_v<U>
 */
template <class U>
struct divisor {

  using uint_t = U;

  /**
   * @brief Creates divisor.
   *
   * @param d   Divisor.
   * @pre       d > 0
   */
  static constexpr divisor
  create(uint_t d) noexcept {

    auto const value        = d;
    auto const multiplier   = math::ceil_sup_divided_by(d);
    auto const p            = math::ceil_log2(d);
    auto const shift        = math::n_bits<uint_t> - p;
    auto const max_dividend = [=]() {

      if (p == math::n_bits<uint_t>)
        return uint_t(0);

      auto const a = math::max<uint_t> / (d - math::remainder_sup_divided_by(d));
      if (a < d - 1)
        return uint_t(0);

      auto const b = a == d - 1 ? a : a - a % d - 1;
      return b >> p;
    }();

    return divisor(value, multiplier, shift, max_dividend);
  }

  /**
   * @brief Implicit conversion operator that returns the divisor's value given
   * at construction time.
   */
  constexpr
  operator uint_t() const noexcept {
    return value;
  }

  uint_t   const value;
  uint_t   const multiplier;
  unsigned const shift;
  uint_t   const max_dividend;

private:

  /**
   * @brief Constructor.
   *
   * @param d   Divisor.
   * @pre       d > 0
   */
  constexpr
  divisor(uint_t value, uint_t multiplier, unsigned shift, uint_t max_dividend)
    noexcept :
    value       (value       ),
    multiplier  (multiplier  ),
    shift       (shift       ),
    max_dividend(max_dividend) {
  }
}; // struct divisor

/**
 * @brief Print divisor's data.
 *
 * @tparam P    Type of p.
 * @tparam U    Divisor's value type.
 * @param  p    Printer.
 * @param  d    Divisor.
 */
template <class P, class U>
void
print(P& p, divisor<U> d) {
  p("value"       , d.value       )
   ("multiplier"  , d.multiplier  )
   ("shift"       , d.shift       )
   ("max_dividend", d.max_dividend)
  ;
}

/**
 * @brief Implementation of the remainder by multiply and right shift algorithm.
 *
 * @tparam U    Divisor's value type.
 * @pre         std::is_unsigned_v<U>
 */
template <class U>
struct algo {

  using uint_t    = U;
  using divisor_t = impl::divisor<uint_t>;

  /**
   * @brief Creates the algorithm for a given divisor.
   *
   * @param d   Divisor.
   * @pre       d > 0
   */
  constexpr explicit
  algo(uint_t d) noexcept : d(divisor_t::create(d)) {
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
    return divisor().max_dividend;
  }

  /**
   * @brief Returns a number f(n % divisor()) where f is an unspecified strict
   * increasing function.
   *
   * @param n   Dividend.
   */
  constexpr uint_t
  mapped_remainder(uint_t n) const noexcept {
    auto const d = divisor();
    return math::rshift(d.multiplier * n, d.shift);
  }

  /**
   * @brief Returns a number f(n % divisor()) where f is an unspecified strict
   * increasing function.
   *
   * @param n   Dividend.
   * @pre       n < divisor()
   */
  constexpr uint_t
  mapped_remainder_bounded(uint_t n) const noexcept {
    return mapped_remainder(n);
  }

private:

  divisor_t const d;

}; // class algo

} // namespace impl

template <class U>
using plain = adaptors::extra_comparison<adaptors::equivalence<
  adaptors::relax_inequality<adaptors::relax_equality<
  adaptors::basic_comparison<impl::algo<U>>>>>>;

template <class U>
using promoted = adaptors::extra_comparison<adaptors::equivalence<
  adaptors::promote_equality<adaptors::promote_inequality<
  adaptors::relax_inequality<adaptors::relax_equality<
  adaptors::basic_comparison<impl::algo<U>>>>>>>>;

} // namespace mshift

template <>
constexpr const char* algo_name<mshift::plain> = "mshift";

template <>
constexpr const char* algo_name<mshift::promoted> = "mshift_promoted";

} // namespace qmodular
