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
 * @file search.cpp
 *
 * @brief A program to make adhoc searches on divisor properties.
 */

#include "built_in.hpp"
#include "meta.hpp"
#include "math.hpp"
#include "minverse.hpp"
#include "mshift.hpp"
#include "new_algo.hpp"

#include <iostream>

int main() {

  using namespace qmodular;

  using uint_t = std::uint32_t;
  auto first   = uint_t(1);
  auto last    = math::max<uint_t>;

  for (auto d = first; d != 0 && d <= last; ++d) {

    auto const divisor = new_algo::plain<uint_t>(d).divisor();

    if (divisor.max_dividend != 0 && divisor.max_dividend != math::max<uint_t>) {
      std::cout << d << '\n';
      return 0;
    }
  }

  return -1;
}
