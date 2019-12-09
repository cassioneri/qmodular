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
 * @file div.cpp
 *
 * @brief A program that outputs information for a given divisor which is
 * relevant to the modular inverse algorithm.
 */

#include <unistd.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ios>
#include <iostream>
#include <string>

#include "built_in.hpp"
#include "mcomp.hpp"
#include "meta.hpp"
#include "minverse.hpp"
#include "mshift.hpp"
#include "new_algo.hpp"

struct terminator {
  void print() {
  }
};

template <class P, class T, class U = terminator>
struct node {

  node(P* printer, const char* name, T const* value, U* prev = nullptr)
    noexcept : printer_(printer), name_(name), value_(value), prev_(prev) {
    printer->inspect(name, *value);
  }

  template <class V>
  node<P, V, node<P, T, U>> operator()(const char* name, V const& value) {
    return {printer_, name, &value, this};
  }

  ~node() {
    print();
  }

  void print() {
    if (prev_)
      prev_->print();
    if (printer_) {
      printer_->print(name_, *value_);
      printer_ = nullptr;
    }
  }

private:

  P*          printer_;
  const char* name_;
  T const*    value_;
  U*          prev_;

}; // struct node

template <class U>
struct column_printer {

  column_printer(std::ostream& os) : os_(os) {
  }

  template <class T>
  node<column_printer, T> operator()(const char* name, T const& value) {
    return node<column_printer, T>{this, name, &value};
  }

  void inspect(const char* name, ...) {
    auto const len = unsigned(std::strlen(name));
    if (len > max_)
      max_ = len;
  }

  template <class T>
  void print(const char* name, T const& value) {

    static const char spaces[] = "                                        ";

    auto const len = unsigned(std::strlen(name));
    os_ << name <<
      (sizeof(spaces) > max_ ? (spaces + sizeof(spaces) - 1 - max_ + len) : "")
      << " : " << value << '\n';
  }

private:

  unsigned      max_ = 0;
  std::ostream& os_;

}; // class column_printer

template <class U>
struct csv_printer {

  csv_printer(std::ostream& os, bool no_header) : os_(os),
    no_header_(no_header) {
  }

  template <class T>
  node<csv_printer, T> operator()(const char* name, T const& value) {
    return node<csv_printer, T>{this, name, &value};
  }

  void inspect(const char* name, ...) {
    if (!no_header_)
      os_ << (n_fields == 0 ? "" : ", ") << name;
    ++n_fields;
  }

  template <class T>
  void print(const char* , T const& value) {
    if (!no_header_ && n_fields != 0) {
      os_ << '\n';
      no_header_ = true;
    }
    os_ << value << (--n_fields == 0 ? "" : ", ");
  }

private:

  std::ostream& os_;
  bool          no_header_;
  unsigned      n_fields = 0;

}; // class csv_printer

template <template <class> class A, class U>
void
print(U d, bool is_csv, bool no_header) {

  auto a = A<U>(d);

  if (is_csv) {
    auto p = csv_printer<U>{std::cout, no_header};
    print(p, a.divisor());
  }
  else {
    auto p = column_printer<U>{std::cout};
    print(p, a.divisor());
  }
}

template <class U>
void
print(const char* prog, const std::string& algo, U d, bool is_csv,
    bool add_header) {

  using namespace qmodular;

  if (algo == "minverse")
    return print<minverse::plain>(d, is_csv, add_header);

  if (algo == "mshift")
    return print<mshift::plain  >(d, is_csv, add_header);

  if (algo == "mcomp")
    return print<mcomp::plain   >(d, is_csv, add_header);

  if (algo == "new_algo")
    return print<new_algo::plain>(d, is_csv, add_header);

  std::cerr << prog << ": invalid algorithm '" << algo << "'\n";
  std::exit(1);
}

int
main(int argc, char* argv[]) {

  auto const prog = argv[0];

  bool is_csv     = false;
  bool is_help    = false;
  bool is_64_bits = false;
  bool no_header  = false;

  int c;

  while ((c = getopt(argc, argv, "chln")) != -1) {
    switch (c) {
      case 'c':
        is_csv = true;
        break;
      case 'h':
        is_help = true;
        break;
      case 'l':
        is_64_bits = true;
        break;
      case 'n':
        is_csv    = true;
        no_header = true;
        break;
      default:
        std::exit(1);
    }
  }

  if (is_help) {
    std::cout << "Usage: " << prog << " [OPTION]... ALGO DIVISOR...\n"
      "    -c      outputs in csv format\n"
      "    -h      shows this message and exists\n"
      "    -l      shows results for 64 bits operands\n"
      "    -n      outputs in csv format with no header\n";
    std::exit(0);
  }

  if (argc - optind < 2) {
    std::cerr << prog << ": missing arguments\n"
      "Try '" << prog << " -h' for more information\n";
    std::exit(1);
  }

  auto const algo = argv[optind];
  auto       i    = optind + 1;
  for (; i < argc; ++i) {

    auto const d = std::atoll(argv[i]);

    if (d == 0) {
      std::cerr << argv[0] << ": invalid divisor " << argv[i] << '\n';
      std::exit(1);
    }

    if (is_64_bits)
      print(prog, algo, std::uint64_t(d), is_csv, no_header);
    else
      print(prog, algo, std::uint32_t(d), is_csv, no_header);

    no_header = true;

    std::cout << '\n';
  }

}
