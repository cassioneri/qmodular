#!/bin/bash

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

function change_value {
  local key=$1
  local value=$2
  sed "s/${key} = .*;$/${key} = ${value};/1" -i time_per_divisor.cpp
}
 
function change_size {
  change_value uint_t uint$1_t
}

function change_function {
  change_value f function::$1
}

function change_arg {
  change_value n2 $1
}

log_file=results/generate.log
rm -rf ${results/generate.log}

sizes="32 64"
functions="has_remainder has_remainder_less has_remainder_less_equal
  has_remainder_greater has_remainder_greater_equal are_equivalent"
args="0 1 -1"

for size in ${sizes}; do
  for f in ${functions}; do
    for arg in ${args}; do

      [[ ${arg} ==  "0" ]] && arg_label=0
      [[ ${arg} ==  "1" ]] && arg_label=1
      [[ ${arg} == "-1" ]] && arg_label=variable

      file=results/${f}-${arg_label}-${size}.csv

      if [[ ! -f ${file} ]]; then

        echo "---------------------------------------------------------------------" | tee -a ${log_file}
        echo "Generating ${file}" | tee -a ${log_file}
        echo "---------------------------------------------------------------------" | tee -a ${log_file}

        change_size     ${size}
        change_function ${f}
        change_arg      ${arg}

        rm -rf time_per_divisor
        make time_per_divisor | tee -a ${log_file}
        ./time_per_divisor --benchmark_out=${file} --benchmark_out_format=csv

      fi
    done
  done
done
