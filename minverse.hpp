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
 * @file minverse.hpp
 *
 * @brief Modular expressions using modular inverse (minverse) algorithm.
 */

#pragma once

#include "adaptors.hpp"
#include "math.hpp"
#include "meta.hpp"

namespace qmodular {
namespace minverse {
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

    auto const value             = d;
    auto const multiplier        = math::modular_inverse(math::odd_part(d));
    auto const rotation          = math::exp2(d);
    auto const special_remainder = - math::odd_part(d) % d;
    auto const quotient_sup      = math::floor_sup_divided_by(d);
    auto const remainder_sup     = math::remainder_sup_divided_by(d);

    return divisor(value, multiplier, rotation, special_remainder, quotient_sup,
      remainder_sup);
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
  unsigned const rotation;
  uint_t   const special_remainder;
  uint_t   const quotient_sup;
  uint_t   const remainder_sup;

private:

  /**
   * @brief Constructor.
   *
   * @param d   Divisor.
   * @pre       d > 0
   */
  constexpr
  divisor(uint_t value, uint_t multiplier, unsigned rotation,
    uint_t special_remainder, uint_t quotient_sup, uint_t remainder_sup)
    noexcept :
    value            (value            ),
    multiplier       (multiplier       ),
    rotation         (rotation         ),
    special_remainder(special_remainder),
    quotient_sup     (quotient_sup     ),
    remainder_sup    (remainder_sup    ) {
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
  p("value"            , d.value            )
   ("multiplier"       , d.multiplier       )
   ("rotation"         , d.rotation         )
   ("special_remainder", d.special_remainder)
   ("quotient_sup"     , d.quotient_sup     )
   ("remainder_sup"    , d.remainder_sup    )
  ;
}

/**
 * @brief Implementation of the modular inverse algorithm.
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
    return math::max<uint_t>;
  }

  /**
   * @brief Returns the maximum allowed input remainder.
   */
  constexpr uint_t
  max_remainder() const noexcept {
    return divisor() - 1;
  }

  /**
   * @brief Returns the result of n % divisor() == r.
   *
   * @param n   Dividend.
   * @param r   Remainder.
   */
  constexpr bool
  has_remainder(uint_t n, uint_t r) const noexcept {

    auto const d          = divisor();
    auto const b          = equivalents_(r);
    auto const is_special = r == d.special_remainder;

    // Do not use 'if constexpr' with __builtin_constant_p:
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=91158#c0
    if (__builtin_constant_p(is_special) && is_special)
      return math::rrotate(d.multiplier * n, d.rotation) >= -b;

    return math::rrotate(d.multiplier * (n - r), d.rotation) <= b - 1;
  }

private:

  /**
   * @brief Returns the number of integers in {0, ..., 2^w - 1} (mod 2^w)
   * which are equivalent to a given remainder r (mod divisor()), where w =
   * math::n_bits<uint_t>.
   *
   * Notice that, when divisor() == 1 we have r == 0 (per pre-condition) and
   * n == r for all n in {0, ..., 2^{w - 1}}. Hence, the number of integers
   * equivalent to r (mod divisor()) is 2^w. Since the returned result is
   * mod 2^w, in this case, this function returns 0.
   *
   * @param r   The remainder.
   * @pre       r < divisor().
   */
  constexpr uint_t
  equivalents_(uint_t r) const noexcept {
    uint_t const extra = r < divisor().remainder_sup;
    return divisor().quotient_sup + extra;
  }

  divisor_t const d;

}; // class algo

} // namespace impl

template <class U>
using plain = adaptors::equivalence<adaptors::relax_equality<impl::algo<U>>>;

} // namespace minverse

template <>
constexpr const char* algo_name<minverse::plain> = "minverse";

} // namespace qmodular
