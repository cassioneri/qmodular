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
 * @file tests/adaptors.hpp
 *
 * @brief Algorithm adaptors tests.
 */

#include <gtest/gtest.h>

#include "adaptors.hpp"
#include "math.hpp"
#include "meta.hpp"

using namespace qmodular;
using namespace qmodular::adaptors;

namespace {

struct to_be_extended {

  using uint_t = unsigned;

  constexpr uint_t
  divisor() const noexcept {
    return 7;
  }

  constexpr uint_t
  max_dividend() const noexcept {
    return 20;
  }

  constexpr uint_t
  mapped_remainder(uint_t n) const noexcept {
    if (n < 7)
      return 2 * n;
    if (n < 14)
      return 2 * (n -  7);
    return 2 * (n - 14);
  }

  constexpr uint_t
  mapped_remainder_bounded(uint_t n) const noexcept {
    return 2 * n;
  }

}; // struct to_be_extended

} // namespace <anonymous>


TEST(adaptors, basic_comparison) {

  using          algo = basic_comparison<to_be_extended>;
  auto constexpr a    = algo();

  static_assert(!does_implement<to_be_extended, function::has_remainder>);
  static_assert( does_implement<algo          , function::has_remainder>);

  static_assert( a.max_remainder() == 6);
  static_assert( a.has_remainder(0, 0));
  static_assert(!a.has_remainder_less(0, 0));

  for (unsigned n = 0; n <= a.max_dividend(); ++n) {
    for (unsigned r = 0; r < 2 * a.divisor(); ++r) {

      EXPECT_EQ(a.has_remainder(n, r)              , n % 7 == r) <<
        "  for n = " << n << " and r = " << r << '.';

      EXPECT_EQ(a.has_remainder_less(n, r)         , n % 7 <  r) <<
        "  for n = " << n << " and r = " << r << '.';
    }
  }
  SUCCEED();
}

TEST(adaptors, extra_comparison) {

  using          algo = extra_comparison<basic_comparison<to_be_extended>>;
  auto constexpr a    = algo();

  static_assert(!does_implement<to_be_extended, function::has_remainder_less>);
  static_assert( does_implement<algo          , function::has_remainder_less>);

  static_assert( a.has_remainder_less_equal(0, 0));
  static_assert(!a.has_remainder_greater(0, 0));
  static_assert( a.has_remainder_greater_equal(0, 0));

  for (unsigned n = 0; n <= a.max_dividend(); ++n) {
    for (unsigned r = 0; r < 2 * a.divisor(); ++r) {

      EXPECT_EQ(a.has_remainder_less_equal(n, r)   , n % 7 <= r) <<
        "  for n = " << n << " and r = " << r << '.';

      EXPECT_EQ(a.has_remainder_greater(n, r)      , n % 7 >  r) <<
        "  for n = " << n << " and r = " << r << '.';

      EXPECT_EQ(a.has_remainder_greater_equal(n, r), n % 7 >= r) <<
        "  for n = " << n << " and r = " << r << '.';
    }
  }
  SUCCEED();
}

TEST(adaptors, equivalence) {

  using          algo = equivalence<basic_comparison<to_be_extended>>;
  auto constexpr a    = algo();

  static_assert(!does_implement<to_be_extended, function::are_equivalent>);
  static_assert( does_implement<algo          , function::are_equivalent>);

  for (unsigned n = 0; n <= a.max_dividend(); ++n) {
    for (unsigned m = 0; m < a.max_dividend(); ++m) {
      EXPECT_EQ(a.are_equivalent(n, m)             , n % 7 == m % 7) <<
        "  for n = " << n << " and m = " << m << '.';
    }
  }
  SUCCEED();
}

namespace {

struct to_be_relaxed {

  using uint_t = unsigned;

  constexpr uint_t
  divisor() const noexcept {
    return 7;
  }

  constexpr uint_t
  max_dividend() const noexcept {
    return 20;
  }

  constexpr uint_t
  has_remainder(uint_t n, uint_t r) const noexcept {
    if (r >= 7)
      return true;
    return n % 7 == r;
  }

  constexpr uint_t
  has_remainder_less(uint_t n, uint_t r) const noexcept {
    if (r >= 7)
      return false;
    return n % 7 < r;
  }

}; // struct to_be_relaxed

} // namespace <anonymous>

TEST(adaptors, relax_equality) {

  using          algo = relax_equality<to_be_relaxed>;
  auto constexpr a    = algo();

  static_assert( a.max_remainder() == a.max_dividend());
  static_assert( a.has_remainder(0, 0));

  for (unsigned n = 0; n <= a.max_dividend(); ++n) {
    for (unsigned r = 0; r < 2 * a.divisor(); ++r) {
      EXPECT_EQ(a.has_remainder(n, r)              , n % 7 == r) <<
        "  for n = " << n << " and r = " << r << '.';
    }
  }
  SUCCEED();
}

TEST(adaptors, relax_inequality) {

  using          algo = relax_inequality<to_be_relaxed>;
  auto constexpr a    = algo();

  for (unsigned n = 0; n <= a.max_dividend(); ++n) {
    for (unsigned r = 0; r < 2 * a.divisor(); ++r) {
      EXPECT_EQ(a.has_remainder_less(n, r)         , n % 7 <  r) <<
        "  for n = " << n << " and r = " << r << '.';
    }
  }
  SUCCEED();
}
