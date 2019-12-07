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
 * @file math.hpp
 *
 * @brief Useful mathematical functions.
 */

#pragma once

namespace qmodular::math {

/**
 * @brief The largest number representable by a given type.
 *
 * @tparam U    The type.
 * @pre         std::is_unsigned_v<U>
 */
template <class U>
constexpr U max = ~U(0);

/**
 * @brief Returns n < m. (Assembly implementation.)
 *
 * Reason for this function: https://stackoverflow.com/q/59117603/1137388
 *
 * @tparam U    The type of n and m.
 * @param  n    1st number.
 * @param  m    2nd number.
 */
template <class U>
constexpr bool
less(U n, U m) noexcept {

  auto const r = n < m;
  if (__builtin_constant_p(r))
    return r;

  return [&] {
    bool r;
    asm("cmp\t{%[m], %[n]|%[n], %[m]}"
      : "=@ccb"(r) : [n]"r"(n), [m]"re"(m) : "cc");
    return r;
  }();
}

/**
 * @brief The number of 1-bits of a given number.
 *
 * @tparam U    The type of the given number.
 * @param  n    The number.
 * @pre         sizeof(U) <= sizeof(long long)
 */
template <class U>
constexpr unsigned
popcount(U n) noexcept {
  return __builtin_popcountll(n);
}

/**
 * @brief The size in bits of a given type.
 *
 * @tparam U    The type.
 * @pre         sizeof(U) <= sizeof(long long)
 */
template <class U>
constexpr unsigned n_bits = popcount(~U(0));

/**
 * @brief Shifts bits to the right.
 *
 * Given a number n of type U and a count c, if c < n_bits<U> then this function
 * returns n >> c. Otherwise, it returns 0. (Notice that n >> c is undefined in
 * the latter case.)
 *
 * @tparam U    Type of the given number.
 * @param  n    Number to have its bits shifted.
 * @param  c    Number of bits to shift.
 * @pre         std::is_unsigned_v<U>
 */
template <class U>
constexpr U
rshift(U n, unsigned c) noexcept {
  return c < n_bits<U> ? n >> c : 0;
}

/**
 * @brief Shifts bits to the left.
 *
 * Given a number n of type U and a count c, if c < n_bits<U> then this function
 * returns n << c. Otherwise, it returns 0. (Notice that n << c is undefined in
 * the latter case.)
 *
 * @tparam U    Type of the given number.
 * @param  n    Number to have its bits shifted.
 * @param  c    Number of bits to shift.
 * @pre         std::is_unsigned_v<U>
 */
template <class U>
constexpr U
lshift(U n, unsigned c) noexcept {
  return c < n_bits<U> ? n << c : 0;
}

/**
 * @brief Rotates bits to the right.
 *
 * Hopefully the compiler will be clever enough to generate a single ror
 * instruction for this code.
 *
 * @tparam U    Type of the given number.
 * @param  n    Number to have its bits shifted.
 * @param  c    Number of bits to shift.
 * @pre         std::is_unsigned_v<U>
 */
template <class U>
constexpr U
rrotate(U n, unsigned c) noexcept {
  c = c % n_bits<U>;
  return c == 0 ? n : (n >> c) | (n << (n_bits<U> - c));
}

/**
 * @brief Returns true if a given number is a power of 2. Otherwise, returns
 * false.
 *
 * @tparam U    Type of the given number.
 * @param  n    Given number.
 * @pre         std::is_unsigned_v<U>
 */
template <class U>
constexpr bool
is_power_of_2(U n) noexcept {
  return !!n & !(n & (n - 1));
}

/**
 * @brief Returns the even part of a given number.
 *
 * More precisely, given a positive integer n, if n = 0, then this function
 * returns 0. Otherwise, if n > 0, then there exist unique positive integers p
 * and o such that n = 2^p * o and o is odd. This function returns 2^p.
 *
 * @tparam U    Type of the given number.
 * @param  n    Given number.
 * @pre         n > 0 && std::is_unsigned_v<U>
 */
template <class U>
constexpr U
even_part(U n) noexcept {
  return n & -n;
}

/**
 * @brief Returns the odd part of a given number.
 *
 * More precisely, given a strictly positive integer n, there exist unique
 * positive integers p and o such that n = 2^p * o and o is odd. This function
 * returns o.
 *
 * @tparam U    Type of the given number.
 * @param  n    Given number.
 * @pre         n > 0 && std::is_unsigned_v<U>
 */
template <class U>
constexpr U
odd_part(U n) noexcept {
  return n / even_part(n);
}

/**
 * @brief Returns the exponent of 2 that appears in the factorisation of a given
 * number.
 *
 * More precisely, given a strictly positive integer n, there exist unique
 * positive integers p and o such that n = 2^p * o and o is odd. This function
 * returns p.
 *
 * @tparam U    Type of the given number.
 * @param  n    Given number.
 * @pre         n > 0 && std::is_unsigned_v<U> && sizeof(U) <= sizeof(long long)
 */
template <class U>
constexpr unsigned
exp2(U n) noexcept {
  return __builtin_ffsll(n) - 1;
}

/**
 * @brief Returns ceil(log_2(n)).
 *
 * More precisely, given a strictly positive number n, there exists a positive
 * integer k such that k - 1 < log_2(n) <= k. In other words, k is the smallest
 * positive integer such that n <= 2^k. This functions returns k. In other
 *
 * @tparam U    Type of the given number.
 * @param  n    Given number.
 * @pre         n > 0 && std::is_unsigned_v<U> && sizeof(U) <= sizeof(long long)
 */
template <class U>
constexpr unsigned
ceil_log2(U n) noexcept {
  return n_bits<long long> - __builtin_clzll(n) - is_power_of_2(n);
}

/**
 * @brief Returns ceil(2^w / d) mod 2^w, where d > 0 is given and w is the
 * number of bits of the type of d.
 *
 * Notice that, when d == 1 we have 2^w / d = 2^w and, since the result is given
 * as mod 2^w, this function returns 0.
 *
 * @tparam U    Type of d.
 * @param  d    Divisor.
 * @pre         d > 0 && std::is_unsigned_v<U>
 */
template <class U>
constexpr U
ceil_sup_divided_by(U d) noexcept {
  return max<U> / d + 1;
}

/**
 * @brief Returns floor(2^w / d) mod 2^w, where d > 0 is given and w is the
 * number of bits of the type of d.
 *
 * Notice that, when d == 1 we have 2^w / d = 2^w and, since the result is given
 * as mod 2^w, this function returns 0.
 *
 * @tparam U    Type of d.
 * @param  d    Divisor.
 * @pre         d > 0 && std::is_unsigned_v<U>
 */
template <class U>
constexpr U
floor_sup_divided_by(U d) noexcept {
  return max<U> / d + is_power_of_2(d);
}

/**
 * @brief Returns the remainder of 2^w / d, where d > 0 is given and w is the
 * number of bits of the type of d.
 *
 * @tparam U    Type of d.
 * @param  d    Divisor.
 * @pre         d > 0 && std::is_unsigned_v<U>
 */
template <class U>
constexpr U
remainder_sup_divided_by(U d) noexcept {
  return -(floor_sup_divided_by(d) * d);
}

/***
 * @brief Calculates the modular inverse of a given number.
 *
 * More precisely, given an odd number n, this function returns a number m such
 * that n * m == 1 (mod 2^w), where w is the number of bits of the type of n.
 *
 * @tparam U    Type of n.
 * @param  n    Given number.
 * @pre         n & 1 == 1 && std::is_unsigned_v<U> && sizeof(U) <= 80
 */
template <class U>
constexpr U
modular_inverse(U n) noexcept {
  // See https://marc-b-reynolds.github.io/math/2017/09/18/ModInverse.html
  U
  m  = 3 * n ^ 2; //  5 bits
  m *= 2 - n * m; // 10 bits
  m *= 2 - n * m; // 20 bits
  m *= 2 - n * m; // 40 bits
  m *= 2 - n * m; // 80 bits
  return m;
}

/**
 * @brief Returns |n - m| where n and m are given.
 *
 * @tparam U    Type of n and m.
 * @param  n    1st number.
 * @param  m    2nd number.
 * @pre         std::is_unsigned_v<U>
 */
template <class U>
constexpr U
abs_diff(U n, U m) noexcept {

  auto const r = n >= m ? n - m : m - n;
  if (__builtin_constant_p(r))
    return r;

  return [&] {
    asm(
      "sub\t{%[m], %[n]|%[n], %[m]}\n\t"
      "sbb\t{%[m], %[m]|%[m], %[m]}\n\t"
      "xor\t{%[m], %[n]|%[n], %[m]}\n\t"
      "sub\t{%[m], %[n]|%[n], %[m]}"
      : [n]"+r"(n), [m]"+r"(m) : : "cc");
    return n;
  }();
}

} // namespace qmodular::math
