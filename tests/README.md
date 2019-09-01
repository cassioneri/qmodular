## Tests

Directory [tests](tests) contains testing code. The build requires
[Google Test](https://github.com/google/googletest) to be installed in a folder
where

    #include <gtest/gtest.h>

works for the compiler and `-lgtest` works for the linker.


 To build and run

    $ make test && ./test

There are quite exhaustive tests. Some run through all possible 32-bit dividiend
values and take minutes to complete.
