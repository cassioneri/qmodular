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
 * @file new_algo.hpp
 *
 * @brief Modular expressions using the "new algorithm".
 *
 * (TODO (CN): Give this algorithm a proper name.)
 */

#pragma once

#include <type_traits>
#include <utility>

#include "adaptors.hpp"
#include "math.hpp"
#include "meta.hpp"

namespace qmodular {
namespace new_algo {
namespace impl {

/**
 * @brief Divisor information.
 *
 * @tparam U    Divisor's value type.
 * @pre         std::is_unsigned_v<U>
 */
template <class U>
struct divisor {

  using uint_t = U;

  /**
   * @brief Creates divisor.
   *
   * @param d   Divisor.
   * @pre       d > 0
   */
  static constexpr divisor
  create(uint_t d) noexcept {

    auto constexpr w          = math::n_bits<uint_t>;
    auto const     max_period = w - math::exp2(d);

    auto const period         = [=]() {
      auto const odd          = math::odd_part(d);
      auto       period       = unsigned(1);
      auto       power        = uint_t(2); // power := 2^period (mod d)
      while (period <= max_period) {
        if (power == 1)
          return period;
        ++period;
        power *= 2;
        if (power >= odd)
          power -= odd;
      }
      return 0u;
    }();

    if (period == 0)
      return divisor(d, 0, 0, 0);

    auto const n_ones         = max_period / period * period;
    auto const multiplier     = (~uint_t(0) << (w - n_ones)) / d;
    auto const shift          = w - n_ones;
    auto const n_points       = (multiplier - 1) >> shift;

    auto const max_dividend   = [=]() {

      auto const max = math::max<uint_t>;
      auto       n   = n_points;
      if (n >= (uint_t(1) << shift))
        return max;
      n = n_points * (uint_t(1) << n_ones);

      if (n > max / d)
        return max;
      n = n * d;

      if (n > -d)
        return max;

      return n + d - 1; // i.e., n_points * n_ones + d - 1
    }();

    return divisor(d, multiplier, shift, max_dividend);
  }

  /**
   * @brief Implicit conversion operator that returns the divisor's value given
   * at construction time.
   */
  constexpr
  operator uint_t() const noexcept {
    return value;
  }

  uint_t   const value;
  uint_t   const multiplier;
  unsigned const shift;
  uint_t   const max_dividend;

private:

  /**
   * @brief Constructor.
   *
   * @param d   Divisor.
   * @pre       d > 0
   */
  constexpr
  divisor(uint_t value, uint_t multiplier, unsigned shift, uint_t max_dividend)
    noexcept :
    value        (value        ),
    multiplier   (multiplier   ),
    shift        (shift        ),
    max_dividend (max_dividend ) {
  }
}; // struct divisor

/**
 * @brief Print divisor's data.
 *
 * @tparam P    Type of p.
 * @tparam U    Divisor's value type.
 * @param  p    Printer.
 * @param  d    Divisor.
 */
template <class P, class U>
void
print(P& p, divisor<U> d) {
  p("value"        , d.value        )
   ("multiplier"   , d.multiplier   )
   ("shift"        , d.shift        )
   ("max_dividend" , d.max_dividend )
  ;
}

/**
 * @brief Implementation of the new algorithm.
 *
 * @tparam U    Divisor's value type.
 * @pre         std::is_unsigned_v<U>
 */
template <class U>
struct algo {

  using uint_t    = U;
  using divisor_t = impl::divisor<uint_t>;

  /**
   * @brief Creates the algorithm for a given divisor.
   *
   * @param d   Divisor.
   * @pre       d > 0
   */
  constexpr explicit
  algo(uint_t d) noexcept : d(divisor_t::create(d)) {
  }

  /**
   * @brief Returns the divisor.
   */
  constexpr divisor_t const&
  divisor() const noexcept {
    return d;
  }

  /**
   * @brief Returns the maximum allowed input dividend.
   */
  constexpr uint_t
  max_dividend() const noexcept {
    return divisor().max_dividend;
  }

  /**
   * @brief Returns the result of n % divisor() == r.
   *
   * @param n   Dividend.
   * @param r   Remainder.
   */
  bool
  has_remainder(uint_t n, uint_t r) const noexcept {

    auto const d = divisor();

    auto const is_unique = r >= (uint_t(1) << d.shift);
    // Do not use 'if constexpr' with __builtin_constant_p:
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=91158#c0
    if (__builtin_constant_p(is_unique) && is_unique)
      return fractional(n) == d.multiplier * r;

    auto const is_zero = r == 0;
    // Do not use 'if constexpr' with __builtin_constant_p:
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=91158#c0
    if (__builtin_constant_p(is_zero) && is_zero)
      return has_remainder_less(n, 1);

    auto const l_bound = d.multiplier * (r - 1) + 1;
    // Do not use 'if constexpr' with __builtin_constant_p:
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=91158#c0
    if (__builtin_constant_p(l_bound)) {

      if constexpr (true) {
        bool res;
        // clang 8.0.0 does not support output flags ("=@ccb").
        // clang trunk (as of 13-Jul-2019) does.
        asm("cmp\t{%[m], %[n]|%[n], %[m]}" : "=@ccb"(res) :
          [n]"r"(fractional(n) - l_bound), [m]"re"(d.multiplier) : "cc");
        return res;
      }
      else {

        // Issues with this full C++ implementation:

        // 1) (uint_t, d) = (std::uint64_t, 19)
        //
        // Generally, immediate values are at most 32 bits long and they are
        // signed extended when used as 64 bits operands. Therefore, 64 bits
        // constants (e.g., d.multiplier) that are not signed extended 32 bits
        // immediate values, must be loaded ('movabs') into a register to be
        // used as operands. This load needs an extra register and increases
        // code size by 10 bytes.
        //
        // Assume d.multiplier constant. In this case, it is a 64 bits constant
        // that cannot be obtained from a signed extended 32 bits immediate
        // value). Hence, d.multiplier must be loaded into register for the
        // comparison below. It turns out that, this load has already been
        // performed for using in the 'mul' instruction done by fractional(n).
        // Unfortunately, GCC 8.2 changes the comparison below to
        //     fractional(n) - l_bound <= d.multiplier - 1.
        // Hence a new constant, d.multiplier - 1, is introduced and this
        // requires a new load which costs an extra register and adds 10 bytes
        // of code.
        //
        // AFAIK, the rationale for GCC's behaviour is exactly mitigating the
        // issue that immediate values can only be 32 bits. Indeed, by giving
        // preference to constants closer to zero, GCC makes more likely that
        // they fit in 32 bits and can be signed extended to 64 bits. In this
        // case GCC changed from d.multiplier to d.multiplier - 1 but because
        // the larger constant was already in a register, GCC ended up shooting
        // itself in the foot.
        return fractional(n) - l_bound < d.multiplier;
      }
    }

    bool c;
    // clang 8.0.0 does not support output flags ("=@ccnc").
    // clang trunk (as of 13-Jul-2019) does.
    asm("sub\t{%[r], %[n]|%[n], %[r]}" : [n]"+r"(n), "=@ccnc"(c)
      : [r]"r"(r) : "cc");
    return c & has_remainder_less(n, 1);
  }

  /**
   * @brief Returns the result of n % divisor() < r.
   *
   * @param n   Dividend.
   * @param r   Remainder.
   */
  bool
  has_remainder_less(uint_t n, uint_t r) const noexcept {
    auto const d = divisor();
    return fractional(n) + d.multiplier <= d.multiplier * r;
  }

private:

  /**
   * @brief Returns an approximation of the fractional part of n / divisor().
   *
   * More precisely, returns an approximation of 2^w * n / divisor() where w is
   * the number of bits of uint_t. The approximation is given by h^k + l where
   * h and l are, respectively, the upper and lower parts of the product
   * divisor().multiplier * n and k = divisor().shift.
   *
   * @param n   The given number.
   */
  uint_t
  fractional(uint_t n) const noexcept {

    auto const d = divisor();

    uint_t h;

    if constexpr (true)
      // clang 8.0.0 does not support %z to get the proper suffix which is
      // compulsory when m (i.e., d.multiplier) is in memory.
      asm("mul{%z[a]}\t%[m]" : [a]"=a,a"(n), "=d,d"(h)
        : "%a,rm"(n), [m]"rm,a"(d.multiplier) : "cc");

    else {

      // Issues with this full C++ implementation:

      // 1) uint_t = std::uint32_t.
      //
      //    GCC 8.2 follows the C++ implementation very closely and performs the
      //    promoted multiplication rather than using 'mul' which gives the
      //    higher and lower part of the product in edx:eax. Although in most
      //    but not all cases (see issue (2) below) the former is faster than
      //    the latter, the built-in implementation uses the latter (probably
      //    for historical reasons from times of 32 bits CPUs). To get a fairer
      //    comparison of the algorithms we use the historical implementation.

      // 2) (uint_t, d) = (std::uint32_t, 0x0000'ffff) or (std::uint64_t,
      //    0xffff'ffff).
      //
      //    In this case, multiplier = 0x0001'0001 or multiplier = 0x1'0000'0001
      //    is a sum of two powers of 2 and GCC 8.2 avoids the multiplication by
      //    using a shift and an addition. This makes the code longer and
      //    slower.

      using promoted = std::conditional_t<std::is_same_v<uint_t, std::uint32_t>,
        std::uint64_t, __uint128_t>;
      auto const p = promoted(d.multiplier) * n;
      h = uint_t(p >> math::n_bits<uint_t>);
      n = uint_t(p);
    }

    return (h << d.shift) + n;
  }

  divisor_t const d;

}; // class algo

} // namespace impl

template <class U>
using plain = adaptors::extra_comparison<adaptors::equivalence<
  adaptors::relax_inequality<adaptors::relax_equality<impl::algo<U>>>>>;

} // namespace new_algo

template <>
constexpr const char* algo_name<new_algo::plain> = "new_algo";

} // namespace qmodular
