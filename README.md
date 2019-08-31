## Introduction

In a glance, **qmodular** implements different algorithms to evaluate modular
expressions of the forms:

1. `n % d == r`
2. `n % d < r`, `n % d <= r`, `n % d > r` and `n % d >= r`
3. `n % d == m % d`

More precisely, the algorithms are:

* Built in (denoted *built_in*);
* Modular inverse (*minverse*);
* Remainder by multiply and right shift (*mshift*); and
* New algorithm<sup id="NewAlgoName">[1](#fNewAlgoName)</sup> (*new_algo*).

The first implementation is nothing more than a wrapper around C/C++ built in
operator `%` and serves as benchmark which others algorithms are compared
against. Others are subjects of a series of articles ([[1]](#minverse),
[[2]](#mshift) and [[3]](#new_algo)) which gives a layman idea of how they work.
Deeper documentation ([[4]](#deep-minverse), [[5]](#deep-mshift), [[6]](#deep-new_algo)) provides mathematical proofs of correctness.

Although possible, **qmodular** is not meant to be used as a production library.
Instead, it is mostly a **C++ research framework for performance comparison of
modular arithmetic algorithms**. Hopefully, compiler writers will use
**qmodular** for their own research and potentially decide to incorporate the
algorithms in their products.

<b id="fNewAlgoName"><sup>1</sup></b> This name is a place holder. At the best of my
knowledge the algorithm is original but it is possible that it is already known
and it has a proper name. If I do discover such name, then I will edit to use it
here. Otherwise, the algorithm will need a proper name. [↩](#NewAlgoName)



## Overview

Each algorithm is implemented in its own header file at **qmodular**'s top level
directory (e.g., `built_in.hpp`) and belongs to its own `namespace` (e.g.,
`::qmodular::built_in`). The same header file and `namespace` might contain
variants of the algorithm. For instance, `built_in::plain` evaluates the
expression

    n % d == m % d

verbatim as above whereas `built_in::distance`'s evaluation is based on the
equivalent expression

    (n >= m ? n - m : m - n) % d == 0

To instantiate an algorithm one must provide the underlying unsigned integer
type of divisor, dividend and remainder as a `template` argument. The divisor's
value must be given at construction. More precisely, an algorithm `A` (e.g.,
`built_in::plain`) for type `U` (e.g., `std::uint32_t`) and divisor `d` is
instantiated as follows:

    A<U> algo(d);

It is worth noticing that `d` can be a runtime variable but construction is, in
general, expensive. Usually, **qmodular** algorithms are instantiated once
(preferably at "compile time" as `constexpr` objects) and used many times.

The following snippet shows the most common usages for the algorithm object
`algo` as constructed above:

    algo.has_remainder(n, r)                    /* n % d == r     */
    algo.has_remainder_less(n, r)               /* n % d <  r     */
    algo.has_remainder_less_or_equal(n, r)      /* n % d <= r     */
    algo.has_remainder_greater(n, r)            /* n % d >  r     */
    algo.has_remainder_greater_or_equal(n, r)   /* n % d >= r     */
    algo.are_equivalent(n, m)                   /* n % d == m % d */

Functions appearing on the left-hand side are collectively called *objective
functions*. Each of them corresponds to an enumerator (with the same name) of
the `enum class function`. Values of this type are used as template arguments
for meta-programming purposes (as we shall see below).

Not all objective functions are implemented for any particular algorithm.
Moreover, those that are might have pre-conditions on dividends `n` and `m` as
well as on remainder `r`. When pre-conditions of an objective function hold, the
value it returns matches that of the corresponding right-hand side (shown as
comments). To query validity of pre-conditions the following constructs can be
used:

    algo.max_dividend()    /* maximum allowed value of dividends  */
    algo.max_remainder()   /* maximum allowed value of remainders */



Indeed, the pre-condition for functions `has_remainder*` is
`n <= algo.max_dividend() && r <= max_remainder()` and that of `are_equivalent`
is `n <= algo.max_dividend() && m <= algo.max_dividend()`.

Similarly, whether a given algorithm `A` implements a certain function `f` or
not is given by the `bool` variable `does_implement<A, function::f>`. 

Notice that all objective functions take two input arguments. Hence,
benchmarking code for any two of these functions will be very similar to one
another. To reduce duplication, benchmarking code do not call any of the
objective functions. Instead, they call the algorithm object itself. The
object's type must implement a callable operator that delegates to an objective
function of our choice. To generate such callable objects a `callable` adaptor
class is used as follows:

    callable<A<U>, function::f> algo(d);

The above instantiates a callable algorithm object `algo` that implements
algorithm `A` for type `U` and whose `operator()` delegates to function `f`. A
more concrete example follows:

    callable<built_in::plain<std::uint32_t>, function::has_remainder> algo(7);

Object `algo` is a functor whose `operator ()` implements `built_in::plain`
algorithm for `std::uint32_t` evaluating function `has_remainder` for divisor 7.
For instance,  `algo(7, 0)` and `algo(8, 1)` evaluates to `true` since 7 has
remainder 0, when divided by 7, and 8 has remainder 1.



## Research tools

The framework also contains benchmarking code and other research tools. This
section explains what is contained in `research` folder, how to build and how to
use them. (Instructions assume the current directory is `research`.)

### `div` : divisors information

This is a command line program that outputs information about a divisor. Build
it with:

    $ make div
    
To see usage:

    $ ./div -h

Example: the below yields `minverse` data for divisor `200`

    $ ./div minverse 200
    value             : 200
    multiplier        : 3264175145
    rotation          : 3
    special_remainder : 71
    quotient_sup      : 21474836
    remainder_sup     : 96

(Quantities above are covered by articles on **qmodular**). By default, results
are for 32-bits unsigned integers and using option `-l` yields results for
64-bits. Option `-c` displays  results in `csv` format. Similarly, option `-n`
generates headless `csv` output. (Useful for appending rows to an existing
file.)

More than one divisor can be given. With `bash` range constructs we can produce
`csv` output for divisors in the given range. For instance,

    $ ./div -c minverse {1..10}
    value, multiplier, rotation, special_remainder, quotient_sup, remainder_sup
    1, 1, 0, 0, 0, 0
    2, 1, 1, 1, 2147483648, 0
    3, 2863311531, 0, 1, 1431655765, 1
    4, 1, 2, 3, 1073741824, 0
    5, 3435973837, 0, 1, 858993459, 1
    6, 2863311531, 1, 1, 715827882, 4
    7, 3067833783, 0, 4, 613566756, 4
    8, 1, 3, 7, 536870912, 0
    9, 954437177, 0, 4, 477218588, 4
    10, 3435973837, 1, 1, 429496729, 6



### `inspection.s` : see generated assembly

The top of `inspection.cpp` sets up the case whose assembly code we are
interested in. (See instructions therein.) For instance,

    using              uint_t = std::uint32_t;
    constexpr function f      = function::has_remainder;
    constexpr uint_t   d      = 14;
    constexpr uint_t   n2     = 3;
    using              algos  = algo_list< /* list of algorithms */ >;

is used to inspect the code of `((std::uint_32_t) n) % 14 == 3` as implemented
by all algorithms appearing in `algo_list`.

Generate `inspection.s` and see the assembly code using

    $ make inspection.s && cat inspection.s



### `time` : benchmarking

The top of `time.cpp` sets up the case we are interested in benchmarking. (See
instructions therein.) For instance,

    using                 uint_t   = std::uint32_t;
    constexpr function    f        = function::has_remainder;
    constexpr std::size_t n_points = 65536;
    constexpr uint_t      d        = 14;
    constexpr uint_t      bound1   = 1000000;
    constexpr uint_t      n2       = 3;
    using                 algos    = algo_list< /* list of algorithms */ >;

is used for benchmarking `((std::uint_32_t) n) % 14 == 3` as implemented by all
algorithms appearing in `algo_list`. The output will be the time taken to
evaluate the expression for `65536` values of `n` are uniformly drawn in
`[0, 1000000]`.

You can either build and run the benchmark locally or use
[quick-bench.com](http://quick-bench.com).

To build locally [Google Benchmark](https://github.com/google/benchmark) is
expected to be installed in a directory where

    #include <benchmark/benchmark.h>

works for the compiler and `-lbenchmark -pthread` works for the linker.

Build and run with

    $ make time && ./time

Alternatively, you can generate a file called `quick_bench` that can be
copy-and-pasted into [quick-bench.com](http://quick-bench.com):

    $ make quick_bench

Be aware that [quick-bench.com](http://quick-bench.com) limits the size of input
at 20000 bytes. The command above also shows the size of `quick_bench` for your
information.

It is worth mentioning that, as it is typical in benchmarking, often the OS
disturbs the measurement and huge spikes in timings might appear.



### `time_per_divisor` : comprehensive benchmarking

The program covered in previous section benchmarks only one divisor. Similar
similar results can be obtained for several divisors in one go by
`time_per_divisor`. However, this must be done locally and, again,
[Google Benchmark](https://github.com/google/benchmark) is expected to be
installed where the compiler and linker can find it.

Similarly to `time.cpp`, the top of `time_per_divisor.cpp` sets up the cases
we are interested in, notably the range of divisors:

    constexpr uint_t first = 1;
    constexpr uint_t last  = 1000;

For each divisor a few template functions are instantiated and, consequently,
compilation time increases with the range. For instance, for range [1, 1000] it
typically takes between 1 and 2 minutes. In addition, if the range is large
enough, compiler limits are reached and compilation fails.

Build with

    $ make time_per_divisor

Run `time_per_divisor` to generate the results. Any command line option of
[Google Benchmark](https://github.com/google/benchmark) can be used. (See their
official documentation for more info.) For instance,

    ./time_per_divisor --benchmark_out_format=csv --benchmark_out=results.csv

generates `results.csv` as well as outputs on the screen.



### `generate_results.sh` : more benchmarking

This script loops through all combinations of

* size : 32 and 64 bits;
* functions `has_remainder`, `has_remainder_less`, `has_remainder_less_equal`,
  `has_remainder_greater`, `has_remainder_greater_equal` and `are_equivalent`;
* remainder (or 2nd dividend for `are_equivalent`): `0`, `1` and variable.

For each combination the script compiles, runs and saves the results of
`time_per_divisor`. The full run takes several hours (a day or so) to complete.



## Results

Results produced by `generate_results.sh` are saved in `csv` format in
`results`.

As mentioned earlier, benchmarking results are subject to noise caused by OS
interruptions and other effects. Therefore, artificial spikes might be present in
these results and they need to be taken with a pinch of salt.



## Tests

Directory `tests` contains testing code. The build requires
[Google Test](https://github.com/google/googletest) to be installed in a folder
where

    #include <gtest/gtest.h>

works for the compiler and `-lgtest` works for the linker.


 To build and run

    $ make test && ./test

There are quite exhaustive tests. Some run through all possible 32-bit dividiend
values and take minutes to complete.



## References


<br id="minverse">[1] Cassio Neri, *Quick modular calculations - Part I*,
to appear.
<br id="mshift">[2] Cassio Neri, *Quick modular calculations - Part II*,
to appear.
<br id="new_algo">[3] Cassio Neri, *Quick modular calculations - Part III*,
to appear.
<br id="deep-minverse">[4] [*On the modular inverse algorithm*.](papers/minverse.pdf)
<br id="deep-mshift">[5] *On the Remainder by multiply and right shift*, to appear.
<br id="deep-new_algo">[6] *A new algorithm for modular calculations*, to appear.

#### Copyright (C) 2019 Cassio Neri

#### Copyright (C) 2019 Cassio Neri

[![Creative Commons License](https://i.creativecommons.org/l/by-sa/4.0/88x31.png)](http://creativecommons.org/licenses/by-sa/4.0)
Documentation material in this repository is licensed under a
[Creative Commons Attribution-ShareAlike 4.0 International License](http://creativecommons.org/licenses/by-sa/4.0).

[![GNU General Public License](https://www.gnu.org/graphics/gplv3-88x31.png)](https://www.gnu.org/licenses/gpl-3.0-standalone.html)
Code material in this repository is licensed under the terms of the 
[GNU General Public License](https://www.gnu.org/licenses/gpl-3.0-standalone.html)
as published by the Free Software Foundation, either version 3 of the License,
or any later version.
