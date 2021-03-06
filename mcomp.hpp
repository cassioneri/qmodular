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
 * @file mcomp.hpp
 *
 * @brief Modular expressions using multiply and compare algorithm.
 *
 * A simplified version of this algorithm has recently appeared in [Lemire]. The
 * main difference is that [Lemire] only considers n % d == r for r = 0, whereas
 * here 0 <= r < d.
 *
 * Essentially, this is the algorithm presented in [Warren] section 10-20.
 * However, the implementation here does not compute remainders but stop when
 * there is enough information for remainder comparison.
 *
 * [Lemire] Lemire, D., Kaser, O., Kurz, N., "Faster Remainder by Direct
 * Computation: Applications to Compilers and Software Libraries.",
 * Software: Practice and Experience 49 (6), 2019.
 *
 * [Warren] Warren, H.S., "Hacker's delight." Addison-Wesley, 2013.
 */

#pragma once

#include "adaptors.hpp"
#include "math.hpp"
#include "meta.hpp"

namespace qmodular {
namespace mcomp {
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

    if (d == 1)
      return divisor(1, 0, 1, math::max<uint_t>);

    auto const value        = d;
    auto const multiplier   = math::ceil_sup_divided_by(d);
    auto const extra        = multiplier * d;
    auto const bound        = extra < multiplier ? multiplier - extra : 0;
    auto const max_dividend = extra == 0 ? math::max<uint_t> :
      extra < multiplier ? (bound - 1) / extra * d + d - 1 : 0;
    return divisor(value, multiplier, bound, max_dividend);
  }

  /**
   * @brief Implicit conversion operator that returns the divisor's value given
   * at construction time.
   */
  constexpr
  operator uint_t() const noexcept {
    return value;
  }

  uint_t const value;
  uint_t const multiplier;
  uint_t const bound;
  uint_t const max_dividend;

private:

  /**
   * @brief Constructor.
   *
   * @param d   Divisor.
   * @pre       d > 0
   */
  constexpr
  divisor(uint_t value, uint_t multiplier, uint_t bound, uint_t max_dividend)
    noexcept :
    value       (value       ),
    multiplier  (multiplier  ),
    bound       (bound       ),
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
   ("bound"       , d.bound       )
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
   * @brief Returns the result of n % divisor() == r.
   *
   * @param n   Dividend.
   * @param r   Remainder.
   */
  constexpr bool
  has_remainder(uint_t n, uint_t r) const noexcept {

    auto const d        = divisor();
    auto const is_r_max = r == d - 1;

    if (__builtin_constant_p(is_r_max)) {
      if (is_r_max)
        return !math::less(d.multiplier * n, d.multiplier * (d - 1));
      return math::less(d.multiplier * (n - r), d.multiplier);
    }

    return math::less(d.multiplier * (n - r), d.bound);
  }

  /**
   * @brief Returns the result of n % divisor() < r.
   *
   * @param n   Dividend.
   * @param r   Remainder.
   */
  constexpr bool
  has_remainder_less(uint_t n, uint_t r) const noexcept {
    auto const d = divisor();
    return math::less(d.multiplier * n, d.multiplier * r);
  }

private:

  divisor_t const d;

}; // class algo

} // namespace impl

template <class U>
using plain = adaptors::extra_comparison<adaptors::equivalence<
  adaptors::relax_inequality<adaptors::relax_equality<impl::algo<U>>>>>;

template <class U>
using promoted = adaptors::extra_comparison<adaptors::equivalence<
  adaptors::promote_equality<adaptors::promote_inequality<
  adaptors::relax_inequality<adaptors::relax_equality<impl::algo<U>>>>>>>;

} // namespace mcomp

template <>
constexpr const char* algo_name<mcomp::plain> = "mcomp";

template <>
constexpr const char* algo_name<mcomp::promoted> = "mcomp_promoted";

} // namespace qmodular
