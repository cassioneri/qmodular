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
 * @file tests/test.hpp
 *
 * @brief Test helpers.
 */

#pragma once

#include <cstdint>
#include <iostream>
#include <utility>

#include <gtest/gtest.h>

// Workarounds for recent changes of macro names.
// https://github.com/google/googletest/commit/3a460a26b7a91abf87af7f31b93d29f930e25c82#diff-33849d1889edae69c83379ea2d79d472
#if !defined(TYPED_TEST_SUITE)
#define TYPED_TEST_SUITE TYPED_TEST_CASE
#endif
#if !defined(TYPED_TEST_SUITE_P)
#define TYPED_TEST_SUITE_P TYPED_TEST_CASE_P
#endif
#if !defined(REGISTER_TYPED_TEST_SUITE_P)
#define REGISTER_TYPED_TEST_SUITE_P REGISTER_TYPED_TEST_CASE_P
#endif
#if !defined(INSTANTIATE_TYPED_TEST_SUITE_P)
#define INSTANTIATE_TYPED_TEST_SUITE_P INSTANTIATE_TYPED_TEST_CASE_P
#endif

#include "built_in.hpp"
#include "meta.hpp"

namespace qmodular::test {

template <unsigned i, auto... ns>
struct get_value;

template <unsigned i, auto n, auto... ns>
struct get_value<i, n, ns...> {
  static auto constexpr value = get_value<i - 1, ns...>::value;
};

template <auto n, auto... ns>
struct get_value<0, n, ns...> {
  static auto constexpr value = n;
};

template <class U, U... args>
struct arg_list {
  template <unsigned i>
  static constexpr auto value = get_value<i, args...>::value;
};

using all_arg_types = ::testing::Types<std::uint32_t, std::uint64_t>;

} // namespace qmodular::test

// These types are in the global namespace to make gtest output more beautiful.
// For instance
//     ... where TypeParam = divisor_remainder<...>
// instead of
//     ... where TypeParam = qmodular::test::divisor_remainder<...>

template <class U, U d, U n>
struct divisor_remainder : qmodular::test::arg_list<U, d, n> {
};

template <class U, U d>
struct divisor : qmodular::test::arg_list<U, d> {
};

/**
 * @brief Test cases that are ran for all argument types.
 */
template <class>
struct for_all_arg_types : ::testing::Test {
};
TYPED_TEST_SUITE(for_all_arg_types, qmodular::test::all_arg_types);

namespace qmodular::test {

auto constexpr constexpr_equiv_funcs = func_list<
  function::has_remainder
>();

auto constexpr all_constexpr_funcs = func_list<
  function::has_remainder,
  function::has_remainder_less,
  function::has_remainder_less_equal,
  function::has_remainder_greater,
  function::has_remainder_greater_equal
>();

/**
 * @brief Type container for functions.
 *
 * @tparam U    Divisor's value type.
 */
template <class U>
using benchmark = built_in::plain<U>;

/**
 * @brief Compile time test of interface conformance.
 *
 * @tparam U    Divisor's value type.
 * @tparam A    Algorithm to be tested.
 * @tparam f    Function to be tested.
 */
template <class U, template <class> class A, function f>
constexpr void
static_interface_test() noexcept {
  static_assert(does_implement<A<U>, f>);
}

/**
 * @brief Compile time test of interface conformance.
 *
 * @tparam U    Divisor's value type.
 * @tparam A    1st algorithm to be tested.
 * @tparam As   Other algorithms to be tested.
 * @tparam f    1st function to be tested.
 * @tparam fs   Other functions to be tested.
 */
template <class U, template <class> class A, template <class> class... As,
  function f, function... fs>
constexpr void
static_interface_test(algo_list<A, As...>, func_list<f, fs...>) noexcept {

  static_interface_test<U, A, f>();

  if constexpr (sizeof...(fs) > 0)
    static_interface_test<U>(algo_list<A>(), func_list<fs...>());

  if constexpr (sizeof...(As) > 0)
    static_interface_test<U>(algo_list<As...>(), func_list<f, fs...>());
}

/**
 * @brief Compile time tests of given algorithm and function.
 *
 * @tparam U    Unsigned integer type of d, n and m.
 * @tparam d    Divisor.
 * @tparam A    Algorithm to be tested.
 * @tparam f    Function to be tested.
 * @tparam n    1st argument.
 * @tparam m    2nd argument.
 * @pre         d > 0 && std::is_unsigned_v<U>
 */
template <class U, U d, template <class> class A, function f, U n, U m>
constexpr void
static_test() noexcept {

  auto constexpr a        = callable<A<U>, f>(d);
  auto constexpr actual   = a(n, m);

  auto constexpr b        = callable<benchmark<U>, f>(d);
  auto constexpr expected = b(n, m);

  // The below is better than testing actual == expected since, in case of
  // failure, one can see the expected value.
  if constexpr (expected)
    static_assert(actual == true);
  else
    static_assert(actual == false);
}

/**
 * @brief Compile time tests of given algorithm and function.
 *
 * @tparam U    Unsigned integer type of d, n and m.
 * @tparam d    Divisor.
 * @tparam A    1st algorithm to be tested.
 * @tparam As   Other algorithms to be tested.
 * @tparam f    1st function to be tested.
 * @tparam fs   Other functions to be tested.
 * @tparam n    1st value of 1st argument.
 * @tparam ns   Other values of 1nd argument.
 * @tparam m    1st value of 2nd argument.
 * @tparam ms   Other values of 2nd argument.
 * @pre         d > 0 && std::is_unsigned_v<U>
 */
template <class U, U d,
  template <class> class A, template <class> class... As, function f,
  function... fs, U n, U... ns, U m, U... ms>
constexpr void
static_test(algo_list<A, As...>, func_list<f, fs...>,
  std::integer_sequence<U, n, ns...>, std::integer_sequence<U, m, ms...>)
  noexcept {

  static_test<U, d, A, f, n, m>();

  if constexpr (sizeof...(ms) >  0)
    static_test<U, d>(algo_list<A>(), func_list<f>(),
      std::integer_sequence<U, n>(), std::integer_sequence<U, ms...>());

  if constexpr (sizeof...(ns) > 0)
    static_test<U, d>(algo_list<A>(), func_list<f>(),
      std::integer_sequence<U, ns...>(), std::integer_sequence<U, m, ms...>());

  if constexpr (sizeof...(fs) > 0)
    static_test<U, d>(algo_list<A>(), func_list<fs...>(),
      std::integer_sequence<U, n, ns...>(), std::integer_sequence<U, m, ms...>());

  if constexpr (sizeof...(As) > 0)
    static_test<U, d>(algo_list<As...>(), func_list<f, fs...>(),
      std::integer_sequence<U, n, ns...>(), std::integer_sequence<U, m, ms...>());
}

/**
 * @brief Compile time tests for small numbers.
 *
 * Given the divisor d, all dividends and remainders in {0, ..., d} are tested.
 *
 * @tparam U    Type of d.
 * @param  d    Divisor.
 * @tparam As   Algorithms to be tested.
 * @tparam fs   Functions to be tested.
 * @pre         std::is_unsigned_v<U>
 */
template <class U , U d, template <class> class... As, function... fs>
constexpr void
static_test_small_numbers(algo_list<As...>, func_list<fs...>) noexcept {
  auto constexpr args = std::make_integer_sequence<U, d + 1> ();
  static_test<U, d>(algo_list<As...>(), func_list<fs...>(), args, args);
}

/**
 * @brief Runtime test of given algorithm and function.
 *
 * If the precondition to run the test case does not hold, the test is not run
 * and this function returns false. Otherwise, the test is ran. If it passes,
 * then this function returns true. Otherwise, the program is aborted with a
 * diagnostic message about the failure.
 *
 * @tparam U    Unsigned integer type of d, n and m.
 * @tparam d    Divisor.
 * @tparam A    Algorithm to be tested.
 * @tparam f    Function to be tested.
 * @param  n    1st argument.
 * @param  m    2nd argument.
 * @pre         d > 0 && std::is_unsigned_v<U>
 */
template <class U, U d, template <class> class A, function f>
bool
dynamic_test(U n, U m) {

  auto constexpr a        = callable<A<U>, f>(d);
  auto const     actual   = a(n, m);
  auto constexpr b        = callable<benchmark<U>, f>(d);
  auto const     expected = b(n, m);

  EXPECT_EQ(actual, expected) <<
    algo_name<A> << ", " << func_name<f> << ", d = " << d << ", n = " << n <<
    (f == function::are_equivalent ? ", m = " : ", r = ") << m << '.';

  return actual == expected;
}

/**
 * @brief Exhaustive runtime 32-bit tests of given algorithms and functions.
 *
 * All values of the 1st argument up to maximum allowed dividend are tested.
 *
 * @tparam d    Divisor.
 * @tparam A    1st algorithm to be tested.
 * @tparam As   Other algorithms to be tested.
 * @tparam f    1st function to be tested.
 * @tparam fs   Other functions to be tested.
 * @param  m    2nd argument.
 * @pre         d > 0
 */
template <std::uint32_t d, template <class> class A,
  template <class> class... As, function f, function... fs>
void
exhaustive_32_bits_tests(std::uint32_t m, algo_list<A, As...>,
  func_list<f, fs...>) {

  auto constexpr max_dividend = A<std::uint32_t>(d).max_dividend();

  auto n = std::uint32_t(0);
  do {
    ASSERT_TRUE((dynamic_test<std::uint32_t, d, A, f>(n, m)));
    ++n;
  } while (n != 0 && n <= max_dividend);

  if constexpr (sizeof...(fs) > 0)
    exhaustive_32_bits_tests<d>(m, algo_list<A>(), func_list<fs...>());

  if constexpr (sizeof...(As) > 0)
    exhaustive_32_bits_tests<d>(m, algo_list<As...>(), func_list<f, fs...>());
}

/**
 * @brief Long runtime 64-bit tests of given algorithms and functions.
 *
 * Uses many values of the 1st argument and just a few of the 2nd.
 *
 * @tparam d    Divisor.
 * @tparam A    Algorithm to be used.
 * @tparam As   Other algorithms to be used.
 * @tparam f    Function to be used.
 * @tparam fs   Other functions to be used.
 * @pre         d > 0
 */
template <std::uint64_t d, template <class> class A,
  template <class> class... As, function f, function... fs>
void
long_64_bits_tests(algo_list<A, As...>, func_list<f, fs...>) {

  for (auto n = std::uint64_t(0); n < std::uint64_t(1) << 32; ++n) {
    ASSERT_TRUE((dynamic_test<std::uint64_t, d, A, f>( n, 0)));
    ASSERT_TRUE((dynamic_test<std::uint64_t, d, A, f>( n, 1)));
  }

  if constexpr (sizeof...(fs) > 0)
    long_64_bits_tests<d>(algo_list<A>(), func_list<fs...>());

  if constexpr (sizeof...(As) > 0)
    long_64_bits_tests<d>(algo_list<As...>(), func_list<f, fs...>());
}

} // namespace qmodular::test
