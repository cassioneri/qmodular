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
 * @file tests/math.cpp
 *
 * @brief Mathematical functions tests.
 */

#include <cstdint>

#include <gtest/gtest.h>

#include "math.hpp"

TEST(math, rshift) {

  using namespace qmodular::math;

  static_assert(rshift(~std::uint32_t(0), 31) == 1);
  static_assert(rshift(~std::uint32_t(0), 32) == 0);
  static_assert(rshift(~std::uint32_t(0), 33) == 0);

  static_assert(rshift(~std::uint64_t(0), 63) == 1);
  static_assert(rshift(~std::uint64_t(0), 64) == 0);
  static_assert(rshift(~std::uint64_t(0), 65) == 0);

  SUCCEED();
}

TEST(math, lshift) {

  using namespace qmodular::math;

  static_assert(lshift(~std::uint32_t(0), 31) == 0x80000000);
  static_assert(lshift(~std::uint32_t(0), 32) == 0);
  static_assert(lshift(~std::uint32_t(0), 33) == 0);

  static_assert(lshift(~std::uint64_t(0), 63) == 0x8000000000000000);
  static_assert(lshift(~std::uint64_t(0), 64) == 0);
  static_assert(lshift(~std::uint64_t(0), 65) == 0);

  SUCCEED();
}

TEST(math, rrotate) {

  using namespace qmodular::math;

  static_assert(rrotate(std::uint32_t(0x1234abcd),  0) == 0x1234abcd);
  static_assert(rrotate(std::uint32_t(0x1234abcd),  4) == 0xd1234abc);
  static_assert(rrotate(std::uint32_t(0x1234abcd), 32) == 0x1234abcd);

  static_assert(rrotate(std::uint64_t(0x1234abcd5678ef09),  0) == 0x1234abcd5678ef09);
  static_assert(rrotate(std::uint64_t(0x1234abcd5678ef09),  4) == 0x91234abcd5678ef0);
  static_assert(rrotate(std::uint64_t(0x1234abcd5678ef09), 64) == 0x1234abcd5678ef09);

  SUCCEED();
}

TEST(math, is_power_of_2) {

  using namespace qmodular::math;

  static_assert(!is_power_of_2(std::uint32_t(0)));
  static_assert( is_power_of_2(std::uint32_t(1)));
  static_assert( is_power_of_2(std::uint32_t(2)));
  static_assert(!is_power_of_2(std::uint32_t(3)));
  static_assert( is_power_of_2(std::uint32_t(4)));
  static_assert(!is_power_of_2(std::uint32_t(5)));
  static_assert(!is_power_of_2(std::uint32_t(6)));
  static_assert(!is_power_of_2(std::uint32_t(7)));
  static_assert( is_power_of_2(std::uint32_t(8)));

  static_assert( is_power_of_2(std::uint32_t(0x10000000)));
  static_assert( is_power_of_2(std::uint32_t(0x20000000)));
  static_assert( is_power_of_2(std::uint32_t(0x40000000)));
  static_assert( is_power_of_2(std::uint32_t(0x80000000)));

  static_assert(!is_power_of_2(std::uint64_t(0)));
  static_assert( is_power_of_2(std::uint64_t(1)));
  static_assert( is_power_of_2(std::uint64_t(2)));
  static_assert(!is_power_of_2(std::uint64_t(3)));
  static_assert( is_power_of_2(std::uint64_t(4)));
  static_assert(!is_power_of_2(std::uint64_t(5)));
  static_assert(!is_power_of_2(std::uint64_t(6)));
  static_assert(!is_power_of_2(std::uint64_t(7)));
  static_assert( is_power_of_2(std::uint64_t(8)));

  static_assert( is_power_of_2(std::uint64_t(0x10000000)));
  static_assert( is_power_of_2(std::uint64_t(0x20000000)));
  static_assert( is_power_of_2(std::uint64_t(0x40000000)));
  static_assert( is_power_of_2(std::uint64_t(0x80000000)));

  SUCCEED();
}

TEST(math, even_part) {

  using namespace qmodular::math;

  static_assert(even_part(std::uint32_t(1)) == 1);
  static_assert(even_part(std::uint32_t(2)) == 2);
  static_assert(even_part(std::uint32_t(3)) == 1);
  static_assert(even_part(std::uint32_t(4)) == 4);
  static_assert(even_part(std::uint32_t(5)) == 1);
  static_assert(even_part(std::uint32_t(6)) == 2);
  static_assert(even_part(std::uint32_t(7)) == 1);
  static_assert(even_part(std::uint32_t(8)) == 8);

  static_assert(even_part(std::uint32_t(100)) ==  4);
  static_assert(even_part(std::uint32_t(200)) ==  8);
  static_assert(even_part(std::uint32_t(300)) ==  4);
  static_assert(even_part(std::uint32_t(400)) == 16);
  static_assert(even_part(std::uint32_t(500)) ==  4);
  static_assert(even_part(std::uint32_t(600)) ==  8);
  static_assert(even_part(std::uint32_t(700)) ==  4);
  static_assert(even_part(std::uint32_t(800)) == 32);

  static_assert(even_part(std::uint64_t(1)) == 1);
  static_assert(even_part(std::uint64_t(2)) == 2);
  static_assert(even_part(std::uint64_t(3)) == 1);
  static_assert(even_part(std::uint64_t(4)) == 4);
  static_assert(even_part(std::uint64_t(5)) == 1);
  static_assert(even_part(std::uint64_t(6)) == 2);
  static_assert(even_part(std::uint64_t(7)) == 1);
  static_assert(even_part(std::uint64_t(8)) == 8);

  static_assert(even_part(std::uint64_t(100)) ==  4);
  static_assert(even_part(std::uint64_t(200)) ==  8);
  static_assert(even_part(std::uint64_t(300)) ==  4);
  static_assert(even_part(std::uint64_t(400)) == 16);
  static_assert(even_part(std::uint64_t(500)) ==  4);
  static_assert(even_part(std::uint64_t(600)) ==  8);
  static_assert(even_part(std::uint64_t(700)) ==  4);
  static_assert(even_part(std::uint64_t(800)) == 32);

  SUCCEED();
}

TEST(math, odd_part) {

  using namespace qmodular::math;

  static_assert(odd_part(std::uint32_t(1)) == 1);
  static_assert(odd_part(std::uint32_t(2)) == 1);
  static_assert(odd_part(std::uint32_t(3)) == 3);
  static_assert(odd_part(std::uint32_t(4)) == 1);
  static_assert(odd_part(std::uint32_t(5)) == 5);
  static_assert(odd_part(std::uint32_t(6)) == 3);
  static_assert(odd_part(std::uint32_t(7)) == 7);
  static_assert(odd_part(std::uint32_t(8)) == 1);

  static_assert(odd_part(std::uint32_t(100)) ==  25);
  static_assert(odd_part(std::uint32_t(200)) ==  25);
  static_assert(odd_part(std::uint32_t(300)) ==  75);
  static_assert(odd_part(std::uint32_t(400)) ==  25);
  static_assert(odd_part(std::uint32_t(500)) == 125);
  static_assert(odd_part(std::uint32_t(600)) ==  75);
  static_assert(odd_part(std::uint32_t(700)) == 175);
  static_assert(odd_part(std::uint32_t(800)) ==  25);

  static_assert(odd_part(std::uint64_t(1)) == 1);
  static_assert(odd_part(std::uint64_t(2)) == 1);
  static_assert(odd_part(std::uint64_t(3)) == 3);
  static_assert(odd_part(std::uint64_t(4)) == 1);
  static_assert(odd_part(std::uint64_t(5)) == 5);
  static_assert(odd_part(std::uint64_t(6)) == 3);
  static_assert(odd_part(std::uint64_t(7)) == 7);
  static_assert(odd_part(std::uint64_t(8)) == 1);

  static_assert(odd_part(std::uint64_t(100)) ==  25);
  static_assert(odd_part(std::uint64_t(200)) ==  25);
  static_assert(odd_part(std::uint64_t(300)) ==  75);
  static_assert(odd_part(std::uint64_t(400)) ==  25);
  static_assert(odd_part(std::uint64_t(500)) == 125);
  static_assert(odd_part(std::uint64_t(600)) ==  75);
  static_assert(odd_part(std::uint64_t(700)) == 175);
  static_assert(odd_part(std::uint64_t(800)) ==  25);

  SUCCEED();
}

TEST(math, exp2) {

  using namespace qmodular::math;

  static_assert(exp2(std::uint32_t(1)) == 0);
  static_assert(exp2(std::uint32_t(2)) == 1);
  static_assert(exp2(std::uint32_t(3)) == 0);
  static_assert(exp2(std::uint32_t(4)) == 2);
  static_assert(exp2(std::uint32_t(5)) == 0);
  static_assert(exp2(std::uint32_t(6)) == 1);
  static_assert(exp2(std::uint32_t(7)) == 0);
  static_assert(exp2(std::uint32_t(8)) == 3);

  static_assert(exp2(std::uint32_t(100)) == 2);
  static_assert(exp2(std::uint32_t(200)) == 3);
  static_assert(exp2(std::uint32_t(300)) == 2);
  static_assert(exp2(std::uint32_t(400)) == 4);
  static_assert(exp2(std::uint32_t(500)) == 2);
  static_assert(exp2(std::uint32_t(600)) == 3);
  static_assert(exp2(std::uint32_t(700)) == 2);
  static_assert(exp2(std::uint32_t(800)) == 5);

  static_assert(exp2(std::uint64_t(1)) == 0);
  static_assert(exp2(std::uint64_t(2)) == 1);
  static_assert(exp2(std::uint64_t(3)) == 0);
  static_assert(exp2(std::uint64_t(4)) == 2);
  static_assert(exp2(std::uint64_t(5)) == 0);
  static_assert(exp2(std::uint64_t(6)) == 1);
  static_assert(exp2(std::uint64_t(7)) == 0);
  static_assert(exp2(std::uint64_t(8)) == 3);

  static_assert(exp2(std::uint64_t(100)) == 2);
  static_assert(exp2(std::uint64_t(200)) == 3);
  static_assert(exp2(std::uint64_t(300)) == 2);
  static_assert(exp2(std::uint64_t(400)) == 4);
  static_assert(exp2(std::uint64_t(500)) == 2);
  static_assert(exp2(std::uint64_t(600)) == 3);
  static_assert(exp2(std::uint64_t(700)) == 2);
  static_assert(exp2(std::uint64_t(800)) == 5);

  SUCCEED();
}

TEST(math, ceil_log2) {

  using namespace qmodular::math;

  static_assert(ceil_log2(std::uint32_t(1)) == 0);
  static_assert(ceil_log2(std::uint32_t(2)) == 1);
  static_assert(ceil_log2(std::uint32_t(3)) == 2);
  static_assert(ceil_log2(std::uint32_t(4)) == 2);
  static_assert(ceil_log2(std::uint32_t(5)) == 3);
  static_assert(ceil_log2(std::uint32_t(6)) == 3);
  static_assert(ceil_log2(std::uint32_t(7)) == 3);
  static_assert(ceil_log2(std::uint32_t(8)) == 3);

  static_assert(ceil_log2(std::uint32_t(100)) ==  7);
  static_assert(ceil_log2(std::uint32_t(200)) ==  8);
  static_assert(ceil_log2(std::uint32_t(300)) ==  9);
  static_assert(ceil_log2(std::uint32_t(400)) ==  9);
  static_assert(ceil_log2(std::uint32_t(500)) ==  9);
  static_assert(ceil_log2(std::uint32_t(600)) == 10);
  static_assert(ceil_log2(std::uint32_t(700)) == 10);
  static_assert(ceil_log2(std::uint32_t(800)) == 10);

  static_assert(ceil_log2(std::uint64_t(1)) == 0);
  static_assert(ceil_log2(std::uint64_t(2)) == 1);
  static_assert(ceil_log2(std::uint64_t(3)) == 2);
  static_assert(ceil_log2(std::uint64_t(4)) == 2);
  static_assert(ceil_log2(std::uint64_t(5)) == 3);
  static_assert(ceil_log2(std::uint64_t(6)) == 3);
  static_assert(ceil_log2(std::uint64_t(7)) == 3);
  static_assert(ceil_log2(std::uint64_t(8)) == 3);

  static_assert(ceil_log2(std::uint64_t(100)) ==  7);
  static_assert(ceil_log2(std::uint64_t(200)) ==  8);
  static_assert(ceil_log2(std::uint64_t(300)) ==  9);
  static_assert(ceil_log2(std::uint64_t(400)) ==  9);
  static_assert(ceil_log2(std::uint64_t(500)) ==  9);
  static_assert(ceil_log2(std::uint64_t(600)) == 10);
  static_assert(ceil_log2(std::uint64_t(700)) == 10);
  static_assert(ceil_log2(std::uint64_t(800)) == 10);

  SUCCEED();
}

TEST(math, ceil_sup_divided_by) {

  using namespace qmodular::math;

  static_assert(ceil_sup_divided_by(std::uint32_t(1)) == 0x00000000);
  static_assert(ceil_sup_divided_by(std::uint32_t(2)) == 0x80000000);
  static_assert(ceil_sup_divided_by(std::uint32_t(3)) == 0x55555556);
  static_assert(ceil_sup_divided_by(std::uint32_t(4)) == 0x40000000);
  static_assert(ceil_sup_divided_by(std::uint32_t(5)) == 0x33333334);
  static_assert(ceil_sup_divided_by(std::uint32_t(6)) == 0x2aaaaaab);
  static_assert(ceil_sup_divided_by(std::uint32_t(7)) == 0x24924925);
  static_assert(ceil_sup_divided_by(std::uint32_t(8)) == 0x20000000);

  static_assert(ceil_sup_divided_by(std::uint32_t(100)) == 0x28f5c29);
  static_assert(ceil_sup_divided_by(std::uint32_t(200)) == 0x147ae15);
  static_assert(ceil_sup_divided_by(std::uint32_t(300)) == 0x0da740e);
  static_assert(ceil_sup_divided_by(std::uint32_t(400)) == 0x0a3d70b);
  static_assert(ceil_sup_divided_by(std::uint32_t(500)) == 0x083126f);
  static_assert(ceil_sup_divided_by(std::uint32_t(600)) == 0x06d3a07);
  static_assert(ceil_sup_divided_by(std::uint32_t(700)) == 0x05d9f74);
  static_assert(ceil_sup_divided_by(std::uint32_t(800)) == 0x051eb86);

  static_assert(ceil_sup_divided_by(std::uint64_t(1)) == 0x0000000000000000);
  static_assert(ceil_sup_divided_by(std::uint64_t(2)) == 0x8000000000000000);
  static_assert(ceil_sup_divided_by(std::uint64_t(3)) == 0x5555555555555556);
  static_assert(ceil_sup_divided_by(std::uint64_t(4)) == 0x4000000000000000);
  static_assert(ceil_sup_divided_by(std::uint64_t(5)) == 0x3333333333333334);
  static_assert(ceil_sup_divided_by(std::uint64_t(6)) == 0x2aaaaaaaaaaaaaab);
  static_assert(ceil_sup_divided_by(std::uint64_t(7)) == 0x2492492492492493);
  static_assert(ceil_sup_divided_by(std::uint64_t(8)) == 0x2000000000000000);

  static_assert(ceil_sup_divided_by(std::uint64_t(100)) == 0x28f5c28f5c28f5d);
  static_assert(ceil_sup_divided_by(std::uint64_t(200)) == 0x147ae147ae147af);
  static_assert(ceil_sup_divided_by(std::uint64_t(300)) == 0x0da740da740da75);
  static_assert(ceil_sup_divided_by(std::uint64_t(400)) == 0x0a3d70a3d70a3d8);
  static_assert(ceil_sup_divided_by(std::uint64_t(500)) == 0x083126e978d4fe0);
  static_assert(ceil_sup_divided_by(std::uint64_t(600)) == 0x06d3a06d3a06d3b);
  static_assert(ceil_sup_divided_by(std::uint64_t(700)) == 0x05d9f7390d2a6c5);
  static_assert(ceil_sup_divided_by(std::uint64_t(800)) == 0x051eb851eb851ec);

  SUCCEED();
}

TEST(math, floor_sup_divided_by) {

  using namespace qmodular::math;

  static_assert(floor_sup_divided_by(std::uint32_t(1)) == 0x00000000);
  static_assert(floor_sup_divided_by(std::uint32_t(2)) == 0x80000000);
  static_assert(floor_sup_divided_by(std::uint32_t(3)) == 0x55555555);
  static_assert(floor_sup_divided_by(std::uint32_t(4)) == 0x40000000);
  static_assert(floor_sup_divided_by(std::uint32_t(5)) == 0x33333333);
  static_assert(floor_sup_divided_by(std::uint32_t(6)) == 0x2aaaaaaa);
  static_assert(floor_sup_divided_by(std::uint32_t(7)) == 0x24924924);
  static_assert(floor_sup_divided_by(std::uint32_t(8)) == 0x20000000);

  static_assert(floor_sup_divided_by(std::uint32_t(100)) == 0x28f5c28);
  static_assert(floor_sup_divided_by(std::uint32_t(200)) == 0x147ae14);
  static_assert(floor_sup_divided_by(std::uint32_t(300)) == 0x0da740d);
  static_assert(floor_sup_divided_by(std::uint32_t(400)) == 0x0a3d70a);
  static_assert(floor_sup_divided_by(std::uint32_t(500)) == 0x083126e);
  static_assert(floor_sup_divided_by(std::uint32_t(600)) == 0x06d3a06);
  static_assert(floor_sup_divided_by(std::uint32_t(700)) == 0x05d9f73);
  static_assert(floor_sup_divided_by(std::uint32_t(800)) == 0x051eb85);

  static_assert(floor_sup_divided_by(std::uint64_t(1)) == 0x0000000000000000);
  static_assert(floor_sup_divided_by(std::uint64_t(2)) == 0x8000000000000000);
  static_assert(floor_sup_divided_by(std::uint64_t(3)) == 0x5555555555555555);
  static_assert(floor_sup_divided_by(std::uint64_t(4)) == 0x4000000000000000);
  static_assert(floor_sup_divided_by(std::uint64_t(5)) == 0x3333333333333333);
  static_assert(floor_sup_divided_by(std::uint64_t(6)) == 0x2aaaaaaaaaaaaaaa);
  static_assert(floor_sup_divided_by(std::uint64_t(7)) == 0x2492492492492492);
  static_assert(floor_sup_divided_by(std::uint64_t(8)) == 0x2000000000000000);

  static_assert(floor_sup_divided_by(std::uint64_t(100)) == 0x28f5c28f5c28f5c);
  static_assert(floor_sup_divided_by(std::uint64_t(200)) == 0x147ae147ae147ae);
  static_assert(floor_sup_divided_by(std::uint64_t(300)) == 0x0da740da740da74);
  static_assert(floor_sup_divided_by(std::uint64_t(400)) == 0x0a3d70a3d70a3d7);
  static_assert(floor_sup_divided_by(std::uint64_t(500)) == 0x083126e978d4fdf);
  static_assert(floor_sup_divided_by(std::uint64_t(600)) == 0x06d3a06d3a06d3a);
  static_assert(floor_sup_divided_by(std::uint64_t(700)) == 0x05d9f7390d2a6c4);
  static_assert(floor_sup_divided_by(std::uint64_t(800)) == 0x051eb851eb851eb);

  SUCCEED();
}

TEST(math, remainder_sup_divided_by) {

  using namespace qmodular::math;

  static_assert(remainder_sup_divided_by(std::uint32_t(1)) == 0);
  static_assert(remainder_sup_divided_by(std::uint32_t(2)) == 0);
  static_assert(remainder_sup_divided_by(std::uint32_t(3)) == 1);
  static_assert(remainder_sup_divided_by(std::uint32_t(4)) == 0);
  static_assert(remainder_sup_divided_by(std::uint32_t(5)) == 1);
  static_assert(remainder_sup_divided_by(std::uint32_t(6)) == 4);
  static_assert(remainder_sup_divided_by(std::uint32_t(7)) == 4);
  static_assert(remainder_sup_divided_by(std::uint32_t(8)) == 0);

  static_assert(remainder_sup_divided_by(std::uint32_t(100)) ==  96);
  static_assert(remainder_sup_divided_by(std::uint32_t(200)) ==  96);
  static_assert(remainder_sup_divided_by(std::uint32_t(300)) == 196);
  static_assert(remainder_sup_divided_by(std::uint32_t(400)) ==  96);
  static_assert(remainder_sup_divided_by(std::uint32_t(500)) == 296);
  static_assert(remainder_sup_divided_by(std::uint32_t(600)) == 496);
  static_assert(remainder_sup_divided_by(std::uint32_t(700)) == 396);
  static_assert(remainder_sup_divided_by(std::uint32_t(800)) ==  96);

  static_assert(remainder_sup_divided_by(std::uint64_t(1)) == 0);
  static_assert(remainder_sup_divided_by(std::uint64_t(2)) == 0);
  static_assert(remainder_sup_divided_by(std::uint64_t(3)) == 1);
  static_assert(remainder_sup_divided_by(std::uint64_t(4)) == 0);
  static_assert(remainder_sup_divided_by(std::uint64_t(5)) == 1);
  static_assert(remainder_sup_divided_by(std::uint64_t(6)) == 4);
  static_assert(remainder_sup_divided_by(std::uint64_t(7)) == 2);
  static_assert(remainder_sup_divided_by(std::uint64_t(8)) == 0);

  static_assert(remainder_sup_divided_by(std::uint64_t(100)) ==  16);
  static_assert(remainder_sup_divided_by(std::uint64_t(200)) ==  16);
  static_assert(remainder_sup_divided_by(std::uint64_t(300)) ==  16);
  static_assert(remainder_sup_divided_by(std::uint64_t(400)) ==  16);
  static_assert(remainder_sup_divided_by(std::uint64_t(500)) == 116);
  static_assert(remainder_sup_divided_by(std::uint64_t(600)) ==  16);
  static_assert(remainder_sup_divided_by(std::uint64_t(700)) ==  16);
  static_assert(remainder_sup_divided_by(std::uint64_t(800)) == 416);

  SUCCEED();
}

TEST(math, modular_inverse) {

  using namespace qmodular::math;

  static_assert(modular_inverse(std::uint32_t(1)) * 1 == 1);
  static_assert(modular_inverse(std::uint32_t(3)) * 3 == 1);
  static_assert(modular_inverse(std::uint32_t(5)) * 5 == 1);
  static_assert(modular_inverse(std::uint32_t(7)) * 7 == 1);

  static_assert(modular_inverse(std::uint32_t(101)) * 101 == 1);
  static_assert(modular_inverse(std::uint32_t(201)) * 201 == 1);
  static_assert(modular_inverse(std::uint32_t(301)) * 301 == 1);
  static_assert(modular_inverse(std::uint32_t(401)) * 401 == 1);
  static_assert(modular_inverse(std::uint32_t(501)) * 501 == 1);
  static_assert(modular_inverse(std::uint32_t(601)) * 601 == 1);
  static_assert(modular_inverse(std::uint32_t(701)) * 701 == 1);
  static_assert(modular_inverse(std::uint32_t(801)) * 801 == 1);

  static_assert(modular_inverse(std::uint64_t(1)) * 1 == 1);
  static_assert(modular_inverse(std::uint64_t(3)) * 3 == 1);
  static_assert(modular_inverse(std::uint64_t(5)) * 5 == 1);
  static_assert(modular_inverse(std::uint64_t(7)) * 7 == 1);

  static_assert(modular_inverse(std::uint64_t(101)) * 101 == 1);
  static_assert(modular_inverse(std::uint64_t(201)) * 201 == 1);
  static_assert(modular_inverse(std::uint64_t(301)) * 301 == 1);
  static_assert(modular_inverse(std::uint64_t(401)) * 401 == 1);
  static_assert(modular_inverse(std::uint64_t(501)) * 501 == 1);
  static_assert(modular_inverse(std::uint64_t(601)) * 601 == 1);
  static_assert(modular_inverse(std::uint64_t(701)) * 701 == 1);
  static_assert(modular_inverse(std::uint64_t(801)) * 801 == 1);

  SUCCEED();
}

TEST(math, abs_diff) {

  using namespace qmodular::math;

  EXPECT_EQ(abs_diff( std::uint32_t( 0), std::uint32_t( 0)),                 0);
  EXPECT_EQ(abs_diff( std::uint32_t( 1), std::uint32_t( 0)),                 1);
  EXPECT_EQ(abs_diff( std::uint32_t( 0), std::uint32_t( 1)),                 1);
  EXPECT_EQ(abs_diff( std::uint32_t(50), std::uint32_t(10)),                40);
  EXPECT_EQ(abs_diff( std::uint32_t(50), std::uint32_t(90)),                40);
  EXPECT_EQ(abs_diff(~std::uint32_t( 0), std::uint32_t( 0)), ~std::uint32_t(0));
  EXPECT_EQ(abs_diff( std::uint32_t( 0),~std::uint32_t( 0)), ~std::uint32_t(0));

  EXPECT_EQ(abs_diff( std::uint64_t( 0), std::uint64_t( 0)),                 0);
  EXPECT_EQ(abs_diff( std::uint64_t( 1), std::uint64_t( 0)),                 1);
  EXPECT_EQ(abs_diff( std::uint64_t( 0), std::uint64_t( 1)),                 1);
  EXPECT_EQ(abs_diff( std::uint64_t(50), std::uint64_t(10)),                40);
  EXPECT_EQ(abs_diff( std::uint64_t(50), std::uint64_t(90)),                40);
  EXPECT_EQ(abs_diff(~std::uint64_t( 0), std::uint64_t( 0)), ~std::uint64_t(0));
  EXPECT_EQ(abs_diff( std::uint64_t( 0),~std::uint64_t( 0)), ~std::uint64_t(0));
}
