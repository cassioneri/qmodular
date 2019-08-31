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
 * @file tests/built_in.hpp
 *
 * @brief Tests of built_in algorithm.
 */

#include <gtest/gtest.h>

#include "meta.hpp"
#include "built_in.hpp"
#include "test.hpp"

/**
 * @brief Test built_in interface conformance.
 */
TYPED_TEST(for_all_arg_types, built_in_interface) {
  using namespace qmodular;
  using uint_t = TypeParam;
  auto constexpr algos = algo_list<built_in::plain, built_in::distance>();
  test::static_interface_test<uint_t>(algos, test::all_constexpr_funcs);
  SUCCEED();
}

TEST(built_in, equivalence) {
  using namespace qmodular;
  auto const a = built_in::distance<std::uint32_t>(5);
  EXPECT_TRUE (a.are_equivalent(0, 0));
  EXPECT_TRUE (a.are_equivalent(0, 5));
  EXPECT_TRUE (a.are_equivalent(5, 0));
  EXPECT_TRUE (a.are_equivalent(1, 6));
  EXPECT_TRUE (a.are_equivalent(6, 1));
  EXPECT_FALSE(a.are_equivalent(5, 6));
  SUCCEED();
}
