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
 * @file tests/newalgo.hpp
 *
 * @brief Tests of new algorithm.
 */

#include <gtest/gtest.h>

#include "meta.hpp"
#include "new_algo.hpp"
#include "test.hpp"

void f() {
}

namespace {

auto constexpr algos = qmodular::algo_list<qmodular::new_algo::plain>();
auto constexpr funcs = qmodular::func_list<qmodular::function::has_remainder>();

} // namespace <anonymous>

/**
 * @brief Test of interface conformance.
 */
TYPED_TEST(for_all_arg_types, new_algo_interface) {
  using namespace qmodular;
  using uint_t = TypeParam;
  test::static_interface_test<uint_t>(algos, test::all_constexpr_funcs);
  SUCCEED();
}

/**
 * @brief Ad-hoc test.
 */
TEST(new_algo, ad_hoc) {

  using namespace qmodular;

  using          uint_t = std::uint32_t;
  auto constexpr d      = uint_t(21);
  auto constexpr f      = function::has_remainder;
  auto const     n      = 1073741845;
  auto const     m      = 1;

  auto const actual   = test::dynamic_test<uint_t, d, new_algo::plain, f>(n, m);
  auto const expected = test::dynamic_test<uint_t, d, built_in::plain, f>(n, m);

  EXPECT_EQ(actual, expected);
}

template <class>
struct new_algo_exhaustive : ::testing::Test {
};

using new_algo_exhaustive_cases = ::testing::Types<
  divisor_remainder<std::uint32_t,      3, 0>,
  divisor_remainder<std::uint32_t,      3, 1>,
  divisor_remainder<std::uint32_t,      3, 2>,
  divisor_remainder<std::uint32_t,      3, 3>,
  divisor_remainder<std::uint32_t,      6, 0>,
  divisor_remainder<std::uint32_t,     19, 0>,
  divisor_remainder<std::uint32_t,  65537, 0>,
  divisor_remainder<std::uint32_t,     38, 0>,
  divisor_remainder<std::uint32_t, 311296, 0>
>;

TYPED_TEST_SUITE(new_algo_exhaustive, new_algo_exhaustive_cases);

/**
 * @brief Exhaustive 32-bit tests.
 */
TYPED_TEST(new_algo_exhaustive, ) {
  using namespace qmodular;
  auto constexpr divisor   = TypeParam::template value<0>;
  auto constexpr remainder = TypeParam::template value<1>;
  test::exhaustive_32_bits_tests<divisor>(remainder, algos, funcs);
  SUCCEED();
}

template <class>
struct new_algo_long : ::testing::Test {
};

using new_algo_long_cases = ::testing::Types<
  divisor<std::uint64_t,  3>,
  divisor<std::uint64_t,  5>
>;

TYPED_TEST_SUITE(new_algo_long, new_algo_long_cases);

/**
 * @brief Long 64-bit tests.
 */
TYPED_TEST(new_algo_long, ) {
  using namespace qmodular;
  auto constexpr divisor = TypeParam::template value<0>;
  test::long_64_bits_tests<divisor>(algos, funcs);
  SUCCEED();
}
