## Research tools

Directory [`research`](research) contains useful research tools for inspecting
algorithms parameters and generated assembly, benchmarking and plotting results,
etc. This section explains how to build and how to use such tools. (Instructions
assume the current directory is `research`.)

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



### `time` : time measurement

The `Config` class at the top of `time.cpp` sets up cases for benchmarking. (See
instructions therein.) For instance,

    uint_t     = std::uint32_t;
    f          = function::has_remainder;
    algos      = algo_list< /* list of algorithms */ >;
    d          = 14;
    n_divisors = 10;
    n_points   = 65536;
    bound      = 1000000;
    n2         = 3;

is used for benchmarking `n % d == 3` (notice `f` and `n2`), as implemented by
all algorithms appearing in `algo_list`, where `n` and `d` have type
`std::uint_32_t` (`uint_t`). The divisor take `10` (`n_divisors`) successive
integer values start from `14` (`d`) whereas `n` takes `65536` (`n_points`)
random values uniformly distributed in `[0, 1000000]` (`bound`).

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
to 20000 bytes. The command above also shows the size of `quick_bench` for your
information. Instructions in the `makefile` try to keep down the file size we
cannot guarantee that this will not be broken in the future.

It is worth mentioning that, as it is typical in benchmarking, often the OS
disturbs the measurement and huge spikes in timings might appear.



### `generate_results.sh` : more benchmarking

This script loops through all combinations of

* size : 32 and 64 bits;
* functions `has_remainder`, `has_remainder_less`, `has_remainder_less_equal`,
  `has_remainder_greater`, `has_remainder_greater_equal` and `are_equivalent`;
* remainder (or 2nd dividend for `are_equivalent`): `0`, `1` and variable.

For each combination the script compiles, runs and saves the results of
`time`. The full run (for `n_points = 65536` and `n_divisors = 1000`) takes
several hours (a day or so) to complete. This have been done and results were
saved in the `research/results` directory. (See next section.)



## Results

Results produced by `generate_results.sh` are saved in `csv` format in
`results`. They are available for the following platforms:

* Ryzen 7 1800X - gcc 8.2.1

As mentioned earlier, benchmarking results are subject to noise caused by OS
interruptions and other effects. Therefore, artificial spikes might be present in
these results and they need to be taken with a pinch of salt.
