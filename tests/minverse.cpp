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
 * @file tests/minverse.hpp
 *
 * @brief Tests of modular inverse algorithm.
 */

#include <gtest/gtest.h>

#include "meta.hpp"
#include "minverse.hpp"
#include "test.hpp"

namespace {

auto constexpr algos = qmodular::algo_list<qmodular::minverse::plain>();
auto constexpr funcs = qmodular::func_list<qmodular::function::has_remainder>();

} // namespace <anonymous>

/**
 * @brief Test of interface conformance.
 */
TYPED_TEST(for_all_arg_types, minverse_interface) {
  using namespace qmodular;
  using uint_t = TypeParam;
  test::static_interface_test<uint_t>(algos, test::constexpr_equiv_funcs);
  SUCCEED();
}

/**
 * @brief Tests for small numbers.
 */
TYPED_TEST(for_all_arg_types, minverse_small_numbers) {
  using namespace qmodular;
  using uint_t = TypeParam;
  test::static_test_small_numbers<uint_t, 1>(algos, test::constexpr_equiv_funcs);
  test::static_test_small_numbers<uint_t, 2>(algos, test::constexpr_equiv_funcs);
  test::static_test_small_numbers<uint_t, 3>(algos, test::constexpr_equiv_funcs);
  test::static_test_small_numbers<uint_t, 4>(algos, test::constexpr_equiv_funcs);
  test::static_test_small_numbers<uint_t, 6>(algos, test::constexpr_equiv_funcs);
  SUCCEED();
}

/**
 * Tests for divisor d = 247808 = 2^11 * 11^2.
 *
 * @tparam U    Divisor's value type.
 * @pre         std::is_unsigned_v<U>
 */
TYPED_TEST(for_all_arg_types, minverse_divisor_is_247808) {

  using namespace qmodular;
  using uint_t = TypeParam;

  auto constexpr n = uint_t(247808);
  auto const args2 = std::integer_sequence<uint_t, 0, 1, 2, n - 1, n, n + 1>();

  test::static_test<uint_t, n>(algos, test::constexpr_equiv_funcs,
    std::integer_sequence<uint_t, 0, 1, 2, n - 1, n, n + 1>(), args2);

  const auto max = math::max<uint_t>;

  test::static_test<uint_t, n>(algos, test::constexpr_equiv_funcs,
    std::integer_sequence<uint_t, max - 2, max - 1, max>(), args2);

  auto constexpr half = max / 2;

  test::static_test<uint_t, n>(algos, test::constexpr_equiv_funcs,
    std::integer_sequence<uint_t, half - 1, half, half + 1>(), args2);

  SUCCEED();
}

template <class>
struct minverse_exhaustive : ::testing::Test {
};

using minverse_exhaustive_cases = ::testing::Types<

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
  divisor_remainder<std::uint32_t, 5, 5>,

  divisor_remainder<std::uint32_t, 4294967295,          0>,
  divisor_remainder<std::uint32_t, 4294967295, 2147483647>,
  divisor_remainder<std::uint32_t, 4294967295, 4294967294>
>;

TYPED_TEST_SUITE(minverse_exhaustive, minverse_exhaustive_cases);

/**
 * @brief Exhaustive 32-bit tests.
 */
TYPED_TEST(minverse_exhaustive, ) {
  using namespace qmodular;
  auto constexpr divisor   = TypeParam::template value<0>;
  auto constexpr remainder = TypeParam::template value<1>;
  test::exhaustive_32_bits_tests<divisor>(remainder, algos, funcs);
  SUCCEED();
}

template <class>
struct minverse_long : ::testing::Test {
};

using minverse_long_cases = ::testing::Types<
  divisor<std::uint64_t,      3>,
  divisor<std::uint64_t,      5>,
  divisor<std::uint64_t, 247808>
>;

TYPED_TEST_SUITE(minverse_long, minverse_long_cases);

/**
 * @brief Long 64-bit tests.
 */
TYPED_TEST(minverse_long, ) {
  using namespace qmodular;
  auto constexpr divisor   = TypeParam::template value<0>;
  test::long_64_bits_tests<divisor>(algos, funcs);
  SUCCEED();
}
