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
 * @file tests/mcomp.hpp
 *
 * @brief Tests of multiply and compare algorithm.
 */

#include <gtest/gtest.h>

#include "meta.hpp"
#include "mcomp.hpp"
#include "test.hpp"

namespace {

auto constexpr algos = qmodular::algo_list<qmodular::mcomp::plain,
  qmodular::mcomp::promoted>();
auto constexpr funcs = qmodular::func_list<qmodular::function::has_remainder>();

} // namespace <anonymous>

/**
 * @brief Test of interface conformance.
 */
TYPED_TEST(for_all_arg_types, mcomp_interface) {
  using namespace qmodular;
  using uint_t = TypeParam;
  test::static_interface_test<uint_t>(algos, test::all_constexpr_funcs);
  SUCCEED();
}

/**
 * @brief Tests for small numbers.
 */
//TYPED_TEST(for_all_arg_types, mcomp_small_numbers) {
//  using namespace qmodular;
//  using uint_t = TypeParam;
//  test::static_test_small_numbers<uint_t, 1>(algos, test::all_constexpr_funcs);
//  test::static_test_small_numbers<uint_t, 2>(algos, test::all_constexpr_funcs);
//  test::static_test_small_numbers<uint_t, 3>(algos, test::all_constexpr_funcs);
//  test::static_test_small_numbers<uint_t, 4>(algos, test::all_constexpr_funcs);
//  test::static_test_small_numbers<uint_t, 6>(algos, test::all_constexpr_funcs);
//  SUCCEED();
//}

template <class>
struct mcomp_exhaustive : ::testing::Test {
};

using mcomp_exhaustive_cases = ::testing::Types<

  divisor_remainder<std::uint32_t, 1, 0>,
  divisor_remainder<std::uint32_t, 1, 1>,

  divisor_remainder<std::uint32_t, 2, 0>,
  divisor_remainder<std::uint32_t, 2, 1>,

  divisor_remainder<std::uint32_t, 3, 0>,
  divisor_remainder<std::uint32_t, 3, 1>,
  divisor_remainder<std::uint32_t, 3, 2>,
  divisor_remainder<std::uint32_t, 3, 3>,

  divisor_remainder<std::uint32_t, 4, 0>,
  divisor_remainder<std::uint32_t, 4, 2>,
  divisor_remainder<std::uint32_t, 4, 4>,

  divisor_remainder<std::uint32_t, 5, 0>,
  divisor_remainder<std::uint32_t, 5, 2>,
  divisor_remainder<std::uint32_t, 5, 5>
>;

TYPED_TEST_SUITE(mcomp_exhaustive, mcomp_exhaustive_cases);

/**
 * @brief Exhaustive 32-bit tests.
 */
TYPED_TEST(mcomp_exhaustive, ) {
  using namespace qmodular;
  auto constexpr divisor   = TypeParam::template value<0>;
  auto constexpr remainder = TypeParam::template value<1>;
  test::exhaustive_32_bits_tests<divisor>(remainder, algos, funcs);
  SUCCEED();
}

template <class>
struct mcomp_long : ::testing::Test {
};

using mcomp_long_cases = ::testing::Types<
  divisor<std::uint64_t, 3>,
  divisor<std::uint64_t, 5>
>;

TYPED_TEST_SUITE(mcomp_long, mcomp_long_cases);

/**
 * @brief Long 64-bit tests.
 */
TYPED_TEST(mcomp_long, ) {
  using namespace qmodular;
  auto constexpr algo    = algo_list<mcomp::plain>();
  auto constexpr divisor = TypeParam::template value<0>;
  test::long_64_bits_tests<divisor>(algo, funcs);
  SUCCEED();
}
