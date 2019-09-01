## Internals

Each algorithm is implemented in its own header file at **qmodular**'s top level
directory (e.g., `built_in.hpp`) and belongs to its own `namespace` (e.g.,
`::qmodular::built_in`). The same header file and `namespace` might contain
variants of the algorithm. For instance, `built_in::plain` evaluates the
expression

    n % d == m % d

verbatim as above whereas `built_in::distance`'s evaluation is based on the
equivalent expression

    (n >= m ? n - m : m - n) % d == 0

Not surprisingly, each algorithm's `namespace` might also contain extra types,
functions or inner `namespace`s that are specific to the algorithm.



### Divisor

For the *built_in* algorithm the divisor is simply a value of `unsigned` integer
type. Other algorithms require extra information (e.g. the divisor's modular
inverse). This information is encapsulated by a type that verify the *divisor
requirements*, that is, a `class` type `D` that contains the following `public`
members.

##### Types

    uint_t

An unsigned integer type used, amongst other things, to hold the
*divisor's value*.

##### Functions

    static constexpr D create(uint_t d) noexcept;

Creates an object of type `D` whose divisor's value is `d`.

    constexpr operator uint_t() const noexcept;

Returns the divisor's value.



### Algorithm

A `class` type `A` is an *algorithm* if it contains the following `public`
members.

##### Types

    uint_t

The `unsigned` integer type of divisor's value.

    divisor_t

The divisor's type. It is either an `unsigned` integer type (e.g. for
*built_in* and its variants) or a type that verify the divisor requirements.

##### Functions

    constexpr explicit A(uint_t d) noexcept;

Constructor taking the divisor's value `d`.

    constexpr divisor_t const& divisor() const noexcept;

Accessor to the algorithm's divisor (constructed by calling
`divisor_t::create`).


    constexpr uint_t max_dividend() const noexcept;

Returns the maximum allowed input dividend.



##### Additional functions

Algorithms might also implement the following functions:

    constexpr uint_t max_remainder() const noexcept;

Returns the maximum allowed input remainder.

    constexpr uint_t mapped_remainder(uint_t n) const noexcept;
    constexpr uint_t mapped_remainder_bounded(uint_t n) const noexcept;

Returns a number `f(n % d)` where `f` is a strict increasing function and `d` is
the divisor's value. The latter assumes `n < d`.



### Underlying algorithms and adaptors

Often a `class` falls short to be an algorithm because it does not implement
some of the compulsory methods. However, provided that some of them are
implemented, *adaptor* classes might extend the `class` to create an algorithm.

A `class` that is expanded by the adaptor is, by abuse of language, called the
*underlying algorithm* (of that adaptor). Other than extending an underlying
algorithm, an adaptor might also change the underlying semantics (e.g., by
relaxing a precondition).

Each adaptor makes specific assumptions on the set of methods that the
underlying algorithm `A` must implement. Generally, they require at least the
existence of types `A::uint_t` and `A::divisor_t` and the following functions:

    constexpr explicit X(uint_t d);
    constexpr divisor_t const& divisor() const noexcept;

The methods that each adaptor requires and those that it implements are
specified in the documentation accompanying its defintion (in its `.hpp` file).
