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

#pragma once

/**
 * @file measure.hpp
 *
 * @brief Measurement functions and classes.
 */

#include "meta.hpp"

namespace qmodular {
namespace measure {

/**
 * @brief A dummy algorithm that does not do anything. Useful to asses the
 * measurement overhead.
 */
template <class U>
struct no_op {
  constexpr no_op(U) noexcept {
  }
  constexpr void
  operator()(U, U) const noexcept {
  }
};

namespace detail {

/**
 * @brief Checks and registers a set of algorithms for measurement.
 *
 * Given a function f and a set of algorithms this function calls a registrar
 * object to book for measurement all algorithms that implement the f.
 *
 * @tparam C          Measurement configuration. (Contains f and divisor's
 *                    type.)
 * @tparam d          Divisor.
 * @tparam R          Type of the registrar.
 * @tparam A          1st algorithm.
 * @tparam As         Other algorithms.
 * @param  registrar  The registrar;
 */
template <class C, typename C::uint_t d, class R, template <class> class A,
  template <class> class... As>
void
register_algos(R& registrar, algo_list<A, As...>) {

  if constexpr (does_implement<A<typename C::uint_t>, C::f>)
    registrar.template book<d, A>();

  if constexpr (sizeof...(As) > 0)
    detail::register_algos<C, d>(registrar, algo_list<As...>());
}

/**
 * @brief Checks and registers a set of algorithms for measurement.
 *
 * Recall that a measurement configuration C contains a number n2 and a function
 * f of two variables and a set of algorithms.
 *
 * When n2 == -1, this function calls a registrar object to book for measurement
 * all algorithms in the set that implement the f.
 *
 * When n2 != -1, then n2 is used as the 2nd argument of f. Provided that
 * n -> f(n, n2) is not constant (e.g., has_remainder_less(n, 0) == false for
 * all n) and a set of algorithms this function calls a registrar object to book
 * for measurement all algorithms in the set that implement the f.
 *
 * @tparam C          Measurement configuration. (Contains f, divisor's type and
 *                    n2.)
 * @tparam d          Divisor.
 * @tparam R          Type of the registrar.
 * @param  registrar  The registrar;
 * @param  points     Contains the array of values used as 1st and (possibly)
 *                    2nd arguments (when they are variable) of the measured
 *                    function.
 */
template <class C, typename C::uint_t d, class R>
void
register_algos(R& registrar) {

  auto constexpr f     = C::f;
  auto constexpr n2    = C::n2;
  auto constexpr valid = n2 == static_cast<typename C::uint_t>(-1) ||
    (f == function::has_remainder               && n2 >= 0 && n2 <= d - 1) ||
    (f == function::has_remainder_less          && n2 >= 1 && n2 <= d - 1) ||
    (f == function::has_remainder_less_equal    && n2 >= 0 && n2 <= d - 2) ||
    (f == function::has_remainder_greater       && n2 >= 0 && n2 <= d - 2) ||
    (f == function::has_remainder_greater_equal && n2 >= 1 && n2 <= d - 1);

  if constexpr (valid)
    detail::register_algos<C, d>(registrar, typename C::algos());
}

/**
 * @brief Registers a set of algorithms for measurement. (Range version.)
 *
 * A given configuration class C informs the objective function (C::f) and the
 * set of algorithms (C::algos) to be measured. The number of divisors and the
 * first of them is also given.
 *
 * This function does not do the registration itself. Instead, it performs some
 * pre-checks and, when they pass, delegates to a registrar object that does the
 * registration.
 *
 * @tparam C          Configuration class.
 * @tparam d          First divisor.
 * @tparam n          Number of divisors.
 * @tparam R          Type of the registrar.
 * @param  registrar  The registrar;
 */
template <class C, typename C::uint_t d, unsigned n, class R>
void
register_algos(R& registrar) noexcept {

  if constexpr (n > 500) {
    register_algos<C, d      , 500    >(registrar);
    register_algos<C, d + 500, n - 500>(registrar);
  }
  else if constexpr (n > 0) {
    register_algos<C, d>(registrar);
    register_algos<C, d + 1, n - 1>(registrar);
  }
}

} // namespace detail

/**
 * @brief Registers a set of algorithms. (Single divisor.)
 *
 * This function does not do the registration itself. Instead, it performs some
 * pre-checks and, when they pass, delegates to a registrar object that does the
 * registration.
 *
 * @tparam C          Configuration containing the list of algorithms, divisor,
 *                    objective function, etc.
 * @tparam d          First divisor.
 * @tparam n          Number of divisors.
 * @tparam R          Type of the registrar.
 * @param  registrar  The registrar;
 */
template <class C, class R>
void
register_1(R& registrar) noexcept {
  detail::register_algos<C, C::d, 1>(registrar);
}

/**
 * @brief Registers a set of algorithms. (Range of divisors.)
 *
 * This function does not do the registration itself. Instead, it performs some
 * pre-checks and, when they pass, delegates to a registrar object that does the
 * registration.
 *
 * @tparam C          Configuration containing the list of algorithms, divisor,
 *                    objective function, etc.
 * @tparam d          First divisor.
 * @tparam n          Number of divisors.
 * @tparam R          Type of the registrar.
 * @param  registrar  The registrar;
 */
template <class C, class R>
void
register_n(R& registrar) noexcept {
  detail::register_algos<C, C::d, C::n_divisors>(registrar);
}

} // namespace measure

template <>
constexpr const char* algo_name<measure::no_op> = "Noop";

} // // namespace qmodular
