 ###############################################################################
 #
 # Copyright (C) 2019 Cassio Neri
 #
 # This file is part of qmodular.
 #
 # qmodular is free software: you can redistribute it and/or modify it under the
 # terms of the GNU General Public License as published by the Free Software
 # Foundation, either version 3 of the License, or (at your option) any later
 # version.
 #
 # qmodular is distributed in the hope that it will be useful, but WITHOUT ANY
 # WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 # A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 #
 # You should have received a copy of the GNU General Public License along with
 # qmodular. If not, see <https://www.gnu.org/licenses/>.
 #
 ###############################################################################

.PHONY      : all All clean

CPP_FILES  = $(wildcard *.cpp)
OBJ_FILES  = $(CPP_FILES:.cpp=.o)
DEP_FILES  = $(CPP_FILES:.cpp=.d)

DEPFLAGS   = -MT $@ -MMD -MP -MF $*.d
CPPFLAGS  += -I.. -std=c++17 -Wall -Werror -pedantic

# If -falign-functions=32 is not used we might get very different and unfair
# time measurements.
# https://easyperf.net/blog/2018/01/18/Code_alignment_issues
CXXFLAGS  += -O3 -falign-functions=32

all         : div inspection.s quick_bench search

All         : all time

time        : LDFLAGS += -lbenchmark -pthread

clean       :
	rm -f $(OBJ_FILES) $(DEP_FILES) div inspection.s quick_bench search time

quick_bench : ../meta.hpp ../math.hpp ../adaptors.hpp \
	../built_in.hpp ../mcomp.hpp ../minverse.hpp ../new_algo.hpp ../mshift.hpp \
	measure.hpp time.cpp
	cat $^ | \
	sed '/#pragma once/d; /#include "/d' | \
	cpp -fpreprocessed -dD -P -E | \
	sed 's/^ *//g;' | \
	sed 's/\bqmodular\b/q/g; s/adaptors/a/g; s/\bdetail\b/d/g; s/\bmath\b/m/g;' | \
	sed 's/\buint_t\b/ut/g; s/divisor_t/dt/g; s/\bfunction\b/ft/g' | \
	sed 's/\bdivisor\b/dv/g; s/\bmax_dividend\b/md/g; s/\bmax_remainder\b/mr/g' | \
	sed 's/\bare_equivalent\b/r0/g; s/\bhas_remainder\b/r1/g' | \
	sed 's/\bhas_remainder_less\b/r2/g; s/\bhas_remainder_less_equal\b/r3/g;' | \
	sed 's/\bhas_remainder_greater\b/r4/g; s/\bhas_remainder_greater_equal\b/r5/g;' | \
	sed 's/remainder/r/g; s/multiplier/m/g; s/quotient/q/g' | \
	sed 's/ *\bexplicit\b *//g; s/ *\bnoexcept\b *//g; /private:/d' | \
	sed 's/ *\([-+\*/%{}&|^,?:()]\) */\1/g; s/ *\([-+\*/!=]=\) */\1/g' | \
	sed 's/ *\([<>]\{1,2\}=\) */\1/g' | \
	sed 's/\([^>]\) *= */\1=/g; s/ *\([<>]\) *\([^=]\)/\1\2/g' \
	> $@
	ls -l $@

%.o         : %.cpp
%.o         : %.cpp %.d
	$(CXX) $(DEPFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $<

$(OBJ_FILES):
$(DEP_FILES):
include $(wildcard $(DEP_FILES))

%.s         : %.o
	objdump -dC --no-show-raw-insn $< | \
	grep '^[0 ]' | \
	sed 's/^.*instantiate_algo<qmodular::\([^>]*\)>.*/\n\1/g' | \
	awk 'BEGIN { p=1 } /^0/ { p=0 } p==1 { print $0 } $$1=="retq" { p=1 }' \
	> $@

%           : %.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
