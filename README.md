## Introduction

In a glance, **qmodular** implements different algorithms to evaluate modular
expressions of the forms:

1. `n % d == r`
2. `n % d < r`, `n % d <= r`, `n % d > r` and `n % d >= r`
3. `n % d == m % d`

More precisely, the algorithms are:

* Built in (denoted *built_in*);
* Modular inverse (*minverse*);
* Multiply and shift (*mshift*);
* Multiply and compare (*mcomp*); and
* New algorithm<sup id="NewAlgoName">[1](#fNewAlgoName)</sup> (*new_algo*).

The first implementation is nothing more than a wrapper around C/C++ built in
operator `%` and serves as benchmark which others algorithms are compared
against. Others are subjects of a series of articles ([[1]](#overload-1),
[[2]](#overload-2) and [[3]](#overload-3)) which gives a layman idea of how they work.
Deeper documentation ([[4]](#deep-1), [[5]](#deep-2), [[6]](#deep-3)) provides mathematical proofs of correctness.

Although possible, **qmodular** is not meant to be used as a production library.
Instead, it is mostly a **C++ research framework for performance comparison of
modular arithmetic algorithms**. Hopefully, compiler writers will use
**qmodular** for their own research and potentially decide to incorporate the
algorithms in their products.

<font size="-1">
<b id="fNewAlgoName"><sup>1</sup></b> This name is a place holder. At the best
of my knowledge the algorithm is original but it is possible that it is already
known and it has a proper name. If I do discover such name, then I will edit to
use it here. In any case, the algorithm will be properly named later.
</font>



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



## Extra stuff

Directory [`research`](research) contains useful research tools for inspecting
algorithms parameters and generated assembly, benchmarking and plotting results,
etc.

Directory [`tests`](tests) contains testing code.

Finally, document [internals](Internals.md) provides more information on how
code is organised.



## References

<span id="overload-1">[1] Cassio Neri, *Quick Modular Calculations (Part 1)*,
to appear in [Overload 154](https://accu.org/index.php/journals), pages 11-15, December 2019.<br>
<span id="overload-2">[2] Cassio Neri, *Quick Modular calculations (Part 2)*,
to appear in [Overload](https://accu.org/index.php/journals/c78/).<br>
<span id="overload-3">[3] Cassio Neri, *Quick Modular calculations (Part 3)*,
to appear in [Overload](https://accu.org/index.php/journals/c78/).<br>
<span id="deep-1">[4] [*On the modular inverse algorithm*.](papers/minverse.pdf)<br>
<span id="deep-2">[5] *On the multiply and shift algorithm for modular calculations*, to appear.<br>
<span id="deep-3">[6] *On a new algorithm for modular calculations*, to appear.<br>



#### Copyright (C) 2019 Cassio Neri

[![Creative Commons License](https://i.creativecommons.org/l/by-sa/4.0/88x31.png)](http://creativecommons.org/licenses/by-sa/4.0)
Documentation material in this repository is licensed under a
[Creative Commons Attribution-ShareAlike 4.0 International License](http://creativecommons.org/licenses/by-sa/4.0).

[![GNU General Public License](https://www.gnu.org/graphics/gplv3-88x31.png)](https://www.gnu.org/licenses/gpl-3.0-standalone.html)
Code material in this repository is licensed under the terms of the 
[GNU General Public License](https://www.gnu.org/licenses/gpl-3.0-standalone.html)
as published by the Free Software Foundation, either version 3 of the License,
or any later version.
