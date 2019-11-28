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

.PHONY      : all clean debug

CPP_FILES  = $(wildcard *.cpp)
OBJ_FILES  = $(CPP_FILES:.cpp=.o)
DEP_FILES  = $(CPP_FILES:.cpp=.d)

DEPFLAGS   = -MT $@ -MMD -MP -MF $*.d
CPPFLAGS  += -I.. -std=c++17 -Wall -Werror -pedantic

# If -falign-functions=32 is not used we might get very different and unfair
# time measurements.
# https://easyperf.net/blog/2018/01/18/Code_alignment_issues
CXXFLAGS  += -O3 -falign-functions=32

LDFLAGS   += -lgtest

all         : test

All         : all

test        : LDFLAGS += -lgtest

%.o         : %.cpp
%.o         : %.cpp %.d
	$(CXX) $(DEPFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $<

$(OBJ_FILES):
$(DEP_FILES):
include $(wildcard $(DEP_FILES))

clean       :
	rm -f $(OBJ_FILES) $(DEP_FILES) test

test        : $(OBJ_FILES)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
