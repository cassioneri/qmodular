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
 * @file meta.hpp
 *
 * @brief Algorithms meta data and type traits.
 */

#pragma once

namespace qmodular {

/**
 * @brief Enumeration of functions that evaluate modular expressions.
 */
enum class function {
  has_remainder,
  has_remainder_less,
  has_remainder_less_equal,
  has_remainder_greater,
  has_remainder_greater_equal,
  are_equivalent,
};

/**
 * @brief Type container for algorithms.
 *
 * @tparam As   The list of algorithms.
 */
template <template <class> class... As>
struct algo_list {
};

/**
 * @brief Type container for functions.
 *
 * @tparam fs   The list of functions.
 */
template <function... fs>
struct func_list {
};

/**
 * @brief Adapts an algorithm into a callable object.
 *
 * Its operator() delegates to the algorithm's function.
 *
 * @tparam A    The underlying algorithm.
 * @tparam f    Function to be delegated to.
 */
template <class A, function f>
struct callable : A {

  using uint_t    = typename A::uint_t;
  using divisor_t = typename A::divisor_t;

  using A::A;
  using A::divisor;

  /**
   * @brief Returns the maximum allowed value of 1st input.
   */
  constexpr uint_t
  max_1st() const noexcept {
    return this->max_dividend();
  }

  /**
   * @brief Returns the maximum allowed value of 2nd input.
   */
  constexpr uint_t
  max_2nd() const noexcept {
    if constexpr (f == function::are_equivalent)
      return max_1st();
    else
      return this->max_remainder();
  }

  /**
   * @brief Delegates to the member function corresponding to f.
   *
   * @param n   1st argument.
   * @param m   2nd argument.
   */
  constexpr bool operator()(uint_t n, uint_t m) const noexcept {
    if constexpr (f == function::has_remainder)
      return this->has_remainder(n, m);
    else if constexpr (f == function::has_remainder_less)
      return this->has_remainder_less(n, m);
    else if constexpr (f == function::has_remainder_less_equal)
      return this->has_remainder_less_equal(n, m);
    else if constexpr (f == function::has_remainder_greater)
      return this->has_remainder_greater(n, m);
    else if constexpr (f == function::has_remainder_greater_equal)
      return this->has_remainder_greater_equal(n, m);
    else if constexpr (f == function::are_equivalent)
      return this->are_equivalent(n, m);
    return false;
  }
};

namespace detail {

/**
 * @brief Equals to true if a given algorithm implements the given function,
 * otherwise equals to false.
 *
 * @tparam A    The algorithm.
 * @tparam f    The function.
 */
template <class A, function f, class = void>
constexpr bool does_implement = false;

template <class A>
constexpr bool does_implement<A, function::has_remainder,
  decltype((void) &A::has_remainder)> = true;

template <class A>
constexpr bool does_implement<A, function::has_remainder_less,
  decltype((void) &A::has_remainder_less)> = true;

template <class A>
constexpr bool does_implement<A, function::has_remainder_less_equal,
  decltype((void) &A::has_remainder_less_equal)> = true;

template <class A>
constexpr bool does_implement<A, function::has_remainder_greater,
  decltype((void) &A::has_remainder_greater)> = true;

template <class A>
constexpr bool does_implement<A, function::has_remainder_greater_equal,
  decltype((void) &A::has_remainder_greater_equal)> = true;

template <class A>
constexpr bool does_implement<A, function::are_equivalent,
  decltype((void) &A::are_equivalent)> = true;

} // namespace detail

/**
 * @brief Template variable that is true if a given algorithm implements the
 * given function or false, otherwise.
 *
 * @tparam A    The algorithm.
 * @tparam f    The function.
 */
template <class A, function f>
constexpr bool does_implement = detail::does_implement<A, f>;

/***
 * @brief Maps an algorithm into its name.
 *
 * Each algorithm must provide an specialisation.
 *
 * @tparam A    The algorithm.
 */
template <template <class> class A>
constexpr const char* algo_name = "";

/***
 * @brief Maps a function into its name.
 *
 * @tparam f    The function.
 */
template <function f>
constexpr const char* func_name = nullptr;

template <>
constexpr const char* func_name<function::has_remainder> = "n % d == r";

template <>
constexpr const char* func_name<function::has_remainder_less> = "n % d <  r";

template <>
constexpr const char* func_name<function::has_remainder_less_equal> =
  "n % d <= r";

template <>
constexpr const char* func_name<function::has_remainder_greater> = "n % d >  r";

template <>
constexpr const char* func_name<function::has_remainder_greater_equal> =
  "n % d >= r";

template <>
constexpr const char* func_name<function::are_equivalent> = "n % d == m % d";

} // namespace qmodular
