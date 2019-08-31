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
 * @file adaptors.hpp
 *
 * @brief Algorithm adaptors.
 */

#pragma once

#include <algorithm>
#include <cstdint>

#include "math.hpp"
#include "meta.hpp"

namespace qmodular::adaptors {

/**
 * @brief Changes the symbol (name) of the underlying algorithm.
 *
 * This is a workaround for the following annoyance. The natural definition of
 * built_in::plain would is:
 *
 *    template <class U>
 *    using plain = impl::algo<U>;
 *
 * However, this yields the symbol built_in::impl::algo in the .o file. With
 *
 *    template <class U>
 *    using plain = adaptors::plain<impl::algo<U>>;
 *
 * the symbol becomes built_in::plain.
 *
 * @tparam A    Underlying algorithm.
 */
template <class A>
struct plain : A {
  using A::A;
};

/**
 * @brief Extends the underlying algorithm by adding basic comparison methods:
 *
 *    max_remainder
 *    has_remainder
 *    has_remainder_less
 *
 * The underlying algorithm must implement:
 *
 *    divisor
 *    max_dividend
 *    mapped_remainder
 *    mapped_remainder_bounded
 *
 * @tparam A    Underlying algorithm.
 */
template <class A>
struct basic_comparison : A {

  using typename A::uint_t;

  using A::A;

  /**
   * @brief Returns the maximum allowed input remainder.
   */
  constexpr uint_t
  max_remainder() const noexcept {
    return std::min(A::max_dividend(), A::divisor() - 1);
  }

  /**
   * @brief Returns the result of n % divisor() == r.
   *
   * @param n   Dividend.
   * @param r   Remainder.
   */
  constexpr bool
  has_remainder(uint_t n, uint_t r) const noexcept {
    return A::mapped_remainder(n) == A::mapped_remainder_bounded(r);
  }

  /**
   * @brief Returns the result of n % divisor() < r.
   *
   * @param n   Dividend.
   * @param r   Remainder.
   */
  constexpr bool
  has_remainder_less(uint_t n, uint_t r) const noexcept {
    return A::mapped_remainder(n) < A::mapped_remainder_bounded(r);
  }

}; // struct basic_comparison

/**
 * @brief Extends the underlying algorithm by adding extra comparison methods:
 *
 *    has_remainder_less_equal
 *    has_remainder_greater
 *    has_remainder_greater_equal
 *
 * The underlying algorithm must implement:
 *
 *    has_remainder_less
 *
 * @tparam A    Underlying algorithm.
 */
template <class A>
struct extra_comparison : A {

  using typename A::uint_t;

  using A::A;

  /**
   * @brief Returns the result of n % divisor() <= r.
   *
   * @param n   Dividend.
   * @param r   Remainder.
   */
  constexpr bool
  has_remainder_less_equal(uint_t n, uint_t r) const noexcept {
    return A::has_remainder_less(n, r + 1);
  }

  /**
   * @brief Returns the result of n % divisor() > r.
   *
   * @param n   Dividend.
   * @param r   Remainder.
   */
  constexpr bool
  has_remainder_greater(uint_t n, uint_t r) const noexcept {
    return !has_remainder_less_equal(n, r);
  }

  /**
   * @brief Returns the result of n % divisor() >= r.
   *
   * @param n   Dividend.
   * @param r   Remainder.
   */
  constexpr bool
  has_remainder_greater_equal(uint_t n, uint_t r) const noexcept {
    return !A::has_remainder_less(n, r);
  }

}; // struct extra_comparison

/**
 * @brief Extends the underlying algorithm by adding the equivalence method:
 *
 *    are_equivalent
 *
 * The underlying algorithm must implement:
 *
 *    divisor
 *    has_remainder
 *
 * @tparam A    Underlying algorithm.
 */
template <class A>
struct equivalence : A {

  using typename A::uint_t;

  using A::A;

  /**
   * @brief Returns the result of n % divisor() == m % divisor().
   *
   * @param n   1st dividend.
   * @param m   2nd dividend.
   */
  constexpr bool
  are_equivalent(uint_t n, uint_t m) const noexcept {

    auto const& d = A::divisor();

    // Do not use 'if constexpr' with __builtin_constant_p:
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=91158#c0
    if (__builtin_constant_p(m % d))
      return A::has_remainder(n, m % d);

    // Do not use 'if constexpr' with __builtin_constant_p:
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=91158#c0
    if (__builtin_constant_p(n % d))
      return A::has_remainder(m, n % d);

    return A::has_remainder(math::abs_diff(n, m), 0);
  }

}; // struct equivalence

/**
 * @brief Relaxes underlying algorithm's precondition on the remainder of the
 * equality comparison method:
 *
 *    has_remainder
 *
 * The underlying algorithm must implement:
 *
 *    divisor
 *    max_dividend
 *    has_remainder
 *
 * @tparam A    Underlying algorithm.
 */
template <class A>
struct relax_equality : A {

  using typename A::uint_t;

  using A::A;

  /**
   * @brief Returns the maximum allowed input remainder.
   */
  constexpr uint_t
  max_remainder() const noexcept {
    return A::max_dividend();
  }

  /**
   * @brief Returns the result of n % divisor() == r.
   *
   * @param n   Dividend.
   * @param r   Remainder.
   */
  constexpr bool
  has_remainder(uint_t n, uint_t r) const noexcept {
    return (r < A::divisor()) & A::has_remainder(n, r);
  }

}; // struct relax_equality

/**
 * @brief Relaxes underlying algorithm's precondition on the remainder of the
 * basic inequality comparison method:
 *
 *    has_remainder_less
 *
 * The underlying algorithm must implement:
 *
 *    divisor
 *    has_remainder_less
 *
 * @tparam A    Underlying algorithm.
 */
template <class A>
struct relax_inequality : A {

  using typename A::uint_t;

  using A::A;

  /**
   * @brief Returns the result of n % divisor() < r.
   *
   * @param n   Dividend.
   * @param r   Remainder.
   */
  constexpr bool
  has_remainder_less(uint_t n, uint_t r) const noexcept {
    return (r >= A::divisor()) | A::has_remainder_less(n, r);
  }

}; // struct relax_inequality

namespace detail {

/**
 * @brief Rebinds a given algorithm to 64 bits.
 *
 * This specialisation stops the recursion started by other one.
 */
template <class T>
struct rebind_to_uint64_t {
  using type = T;
};

/**
 * @brief Rebinds a given algorithm to 64 bits.
 *
 * This specialisation recursively rebinds underlying algorithms.
 */
template <template <class> class A, class T>
struct rebind_to_uint64_t<A<T>> {
  using type = std::conditional_t<std::is_integral<T>{}, A<std::uint64_t>,
    A<typename rebind_to_uint64_t<T>::type>
  >;
};

} // namespace detail

/**
 * @brief Relaxes underlying algorithm's precondition on dividends, by
 * promoting calculations to 64-bits, of the equality comparison method:
 *
 *    has_remainder
 *
 * The underlying algorithm must implement:
 *
 *    max_dividend
 *    max_remainder
 *    has_remainder
 *    are_equivalent
 *
 * @tparam A    Underlying algorithm.
 */
template <class A>
struct promote_equality : public detail::rebind_to_uint64_t<A>::type {

  using base   = typename detail::rebind_to_uint64_t<A>::type;
  using uint_t = typename A::uint_t;

  using base::base;

  /**
   * @brief Returns the maximum allowed input dividend.
   */
  constexpr uint_t
  max_dividend() const noexcept {
    std::uint64_t constexpr m = math::max<uint_t>;
    return std::min(base::max_dividend(), m);
  }

  /**
   * @brief Returns the maximum allowed input remainder.
   */
  constexpr uint_t
  max_remainder() const noexcept {
    std::uint64_t constexpr m = math::max<uint_t>;
    return std::min(base::max_remainder(), m);
  }

  /**
   * @brief Returns the result of n % divisor() == r.
   *
   * @param n   Dividend.
   * @param r   Remainder.
   */
  constexpr bool
  has_remainder(uint_t n, uint_t r) const noexcept {
    return base::has_remainder(n, r);
  }

}; // struct promote_equality

/**
 * @brief Relaxes underlying algorithm's precondition on dividends, by
 * promoting calculations to 64-bits, of the equality comparison method:
 *
 *    has_remainder_less
 *
 * The underlying algorithm must implement:
 *
 *    max_dividend
 *    max_remainder
 *    has_remainder
 *    are_equivalent
 *
 * @tparam A    Underlying algorithm.
 */
template <class A>
struct promote_inequality : public detail::rebind_to_uint64_t<A>::type {

  using base   = typename detail::rebind_to_uint64_t<A>::type;
  using uint_t = typename A::uint_t;

  using base::base;

  /**
   * @brief Returns the result of n % divisor() == r.
   *
   * @param n   Dividend.
   * @param r   Remainder.
   */
  constexpr bool
  has_remainder(uint_t n, uint_t r) const noexcept {
    return base::has_remainder(n, r);
  }

}; // struct promote_inequality

} // namespace qmodular::adaptors
