## Research tools

Directory [research](research) contains useful research tools for inspecting
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
`results`. They are available for the following platforms:

* Ryzen 7 1800X - gcc 8.2.1

As mentioned earlier, benchmarking results are subject to noise caused by OS
interruptions and other effects. Therefore, artificial spikes might be present in
these results and they need to be taken with a pinch of salt.
