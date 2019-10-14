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

#include <random>
#include <string>
#include <vector>

#include "meta.hpp"

namespace qmodular {
namespace measure {

/**
 * @brief Container of test data.
 *
 * Each test case is given by a pair of numbers (n1, n2). Each pair is passed to
 * the function being measured.
 *
 * @tparam U    Type of numbers.
 */
template <class U>
struct data {

  struct args {
    U n1;
    U n2;
  };

  /**
   * @brief Constructor.
   *
   * @param size    Number of test cases.
   * @param bounds  1st argument is drawn in [0, bounds.n1];
   *                2nd argument is drawn in [0, bounds.n2].
   */
  data(unsigned size, args bounds) noexcept :
    bounds_(bounds) {

    data_.resize(size);

    std::mt19937 rng;
    rng.seed(std::random_device()());

    std::uniform_int_distribution<U> d1(0, bounds.n1);
    std::uniform_int_distribution<U> d2(0, bounds.n2);

    for (std::size_t i = 0; i < size; ++i) {
      data_[i].n1 = d1(rng);
      data_[i].n2 = d2(rng);
    }
  }

  /**
   * @brief Gets the arguments bounds.
   */
  args bounds() const noexcept {
    return bounds_;
  }

  /**
   * @brief Accesses the i-th test case.
   *
   * @param i     Index.
   */
  const args& operator [](std::size_t i) const noexcept {
    return data_[i];
  }

  /**
   * @brief Returns the number of test cases.
   */
  std::size_t size() const noexcept {
    return data_.size();
  }

private:

  std::vector<args> data_;
  args const        bounds_;

}; // struct data

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
 * Given a function f and a set of algorithms this function registers for
 * measurement each algorithm in the set that passes some checks namely:
 *
 * 1) The algorithm must implement the f.
 *
 * 2) The data set must satisfy implementation's preconditions. Each algorithm
 *    that implement f might have preconditions on its arguments. Generally, the
 *    1st and 2nd arguments must be in some intervals, say, [0, c1] and [0, c2],
 *    respectively. However, when their values are not fixed they are taken from
 *    input argument points and belong to intervals [0, b1] and [0, b2]. If
 *    b1 > c1 or b2 > c2, then the algorithm is also disregarded to avoid using
 *    values that do not fulfil the precondition.
 *
 * This function does not do the registration itself. Instead, it checks all the
 * conditions above and when they are fulfilled, this functions calls a
 * registrar object that does the registration.
 *
 * @tparam C          Measurement configuration. (Contains f and divisor's
 *                    type and f's 2nd argument when it is constant.)
 * @tparam d          Divisor.
 * @tparam R          Type of the registrar.
 * @tparam A          1st algorithm.
 * @tparam As         Other algorithms.
 * @param  registrar  The registrar;
 * @param  points     Contains the array of values used as 1st and (possibly)
 *                    2nd arguments (when they are variable) of the measured
 *                    function.
 */
template <class C, typename C::uint_t d, class R, template <class> class A,
  template <class> class... As>
void
register_algos(R& registrar, data<typename C::uint_t> const& points,
    algo_list<A, As...>) {

  using U = typename C::uint_t;
  if constexpr (does_implement<A<U>, C::f>) {

    using          algo = callable<A<U>, C::f>;
    auto constexpr a    = algo(d);
    auto const     b    = points.bounds();

    if (b.n1 <= a.max_1st() && b.n2 <= a.max_2nd()) {
      auto const name = std::string(algo_name<A>) + '<' + std::to_string(d) +
        '>';
      registrar.template book<C::n2, d, algo>(name.c_str(), points);
    }
  }

  if constexpr (sizeof...(As) > 0)
    detail::register_algos<C, d>(registrar, points, algo_list<As...>());
}

/**
 * @brief Checks and registers a set of algorithms for measurement.
 *
 * Given a function f and a set of algorithms this function registers for
 * measurement each algorithm in the set that passes some checks namely:
 *
 * 1) Function f must not be constant. For instance, has_remainder_less(n, 0) ==
 *    false for all n whereas has_remainder_greater_equal(n, 0) == true for all
 *    n. Notice that such functions might be constant only if the 2nd argument
 *    is. When f is constant, all algorithms are disregarded.
 *
 * This function does not do the registration itself. Instead, it checks all the
 * conditions above and when they are fulfilled, it calls the other overload
 * above passing some of the input arguments and others.
 *
 * @tparam C          Measurement configuration. (Contains f and divisor's
 *                    type and f's 2nd argument when it is constant.)
 * @tparam R          Type of the registrar.
 * @param  registrar  The registrar;
 * @param  points     Contains the array of values used as 1st and (possibly)
 *                    2nd arguments (when they are variable) of the measured
 *                    function.
 */
template <class C, typename C::uint_t d, class R>
void
register_algos(R& registrar, data<typename C::uint_t> const& points) {

  auto constexpr f     = C::f;
  auto constexpr n2    = C::n2;
  auto constexpr valid = n2 == static_cast<typename C::uint_t>(-1) ||
    (f == function::has_remainder               && n2 >= 0 && n2 <= d - 1) ||
    (f == function::has_remainder_less          && n2 >= 1 && n2 <= d - 1) ||
    (f == function::has_remainder_less_equal    && n2 >= 0 && n2 <= d - 2) ||
    (f == function::has_remainder_greater       && n2 >= 0 && n2 <= d - 2) ||
    (f == function::has_remainder_greater_equal && n2 >= 1 && n2 <= d - 1) ||
    f == function::are_equivalent;

  if constexpr (valid)
    detail::register_algos<C, d>(registrar, points, typename C::algos());
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

    auto constexpr bound2 = C::f == function::are_equivalent ? C::bound : d - 1;
    auto const     points = data<typename C::uint_t >(C::n_points,
        {C::bound, bound2});

    if constexpr (d == C::d)
      registrar.template book_no_op<C::n2>(points);

    register_algos<C, d>(registrar, points);
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
