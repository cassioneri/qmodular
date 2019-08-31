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

CPPFLAGS  += -I.. -std=c++17 -Wall -Werror -pedantic
CXXFLAGS  += -O3 -falign-functions=32

ifeq ($(MAKECMDGOALS),debug)
	CXXFLAGS += -g
endif

all         : test

All         : all

debug       : all

test        : LDFLAGS += -lgtest

ifneq ($(MAKECMDGOALS),clean)
include $(DEP_FILES)
endif

clean       :
	rm -f $(OBJ_FILES) $(DEP_FILES) test

test        : $(OBJ_FILES)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.d         : %.cpp
	$(CPP) $(CPPFLAGS) -MM $^ > $@
