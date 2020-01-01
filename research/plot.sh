#! /bin/bash

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

program=$(basename $0)

function show_usage() {
  echo "Usage: ${program} [OPTION]... SOURCE [ALGO]..."
  echo "Plots charts for each ALGO from a .csv source file SOURCE."
  echo "If no algorithm is provided, then all are used."
  echo ""
  echo "Mandatory arguments to long options are mandatory for short options too."
  echo "  -c, --colour               plot in colours"
  echo "  -f, --filter               filter out power of two divisors"
  echo "  -h, --help                 display this help and exit"
  echo "  -o, --out[=OUTFILE]        ouput to a given file (rather than screen); if OUTFILE is"
  echo "                               not provided, then the same file name as SOURCE is used"
  echo "                               but with .png extension"
  echo "  -r, --relative[=BASELINE]  plot relative (rather than absolut) results with respect to"
  echo "                                BASELINE algorithm (default is 'built_in')"
  echo "  -y, --range=MIN:MAX        set y-axis to [MIN, MAX]"
  exit 0
}

set -- $(getopt --unquoted --options "cfho::r::y::" \
  --longoptions "colour,filter,help,out::,relative::,range::" --name ${program} -- "$@")

relative=0
use_colours=0
to_file=0
filter="1"
range=""

while true; do
    case "$1" in
      -c|--colour)
        use_colours=1
        ;;
      -f|--filter)
        filter="and(\$1, \$1 - 1) != 0"
        ;;
      -h|--help)
        show_usage
        ;;
      -o|--out)
        to_file=1
        if [[ $2 != "--" ]]; then
            out_file=$2
            shift
        fi
        ;;
      -r|--relative)
        relative=1
        if [[ $2 != "--" ]]; then
          baseline=$2
          shift
        fi
        ;;
      -y|--range)
        range="[$2]"
        shift
        ;;
      --)
        shift
        break
        ;;
    esac
    shift
done

if [[ $# -lt 1 ]]; then
  echo "${program}: missing option or file operand" > /dev/stderr
  echo "Try '${program} --help' for more information." > /dev/stderr
  exit 1
fi

#-------------------------------------------------------------------------------
# Config
#-------------------------------------------------------------------------------

in_file=$1
shift

out_file=${out_file:-$(basename -s .csv ${in_file}).png}

baseline=${baseline:=built_in}

algos=${*:-$(sed -e '/^[^"]/d; /Noop/d; s/^"\([a-z_]*\).*/\1/g' ${in_file} | \
  sort -u | tr "\n" " ")}

last_divisor=$(tail -1 ${in_file} | sed 's/.*<\([0-9]*\)>.*/\1/g')
noop=$(grep --max-count=1 Noop ${in_file} | cut -d, -f4)

#-------------------------------------------------------------------------------
# Gnuplot
#-------------------------------------------------------------------------------

/usr/bin/gnuplot -persist <<EOF

#-------------------------------------------------------------------------------
# Data
#-------------------------------------------------------------------------------

awk(algo) = "awk 'BEGIN { FS=\",\" } /\"" . algo . \
  "</ { sub(/^.*</, \"\"); sub(/>\"/, \"\"); " . \
  "if (${filter}) { print \$1 \" \" \$4 }}' ${in_file}"

array baseline[${last_divisor}]

if (!${relative}) {

  do for [i=1:${last_divisor}] {
    baseline[i] = 1
  }

}
else {

  do for [i=1:${last_divisor}] {
    baseline[i] = NaN
  }

  baseline_data = system(awk("${baseline}"))

  is_divisor = 1
  do for [line in baseline_data ] {
    if (is_divisor) { divisor = line }
    else            { baseline[divisor] = line - ${noop} }
    is_divisor = !is_divisor
  }
}

#-------------------------------------------------------------------------------
# Axis, grid and keys
#-------------------------------------------------------------------------------

set tics nomirror
set xtics  5
set mxtics 5
set xrange [0:50]

if (${relative}) {
    set ytics  0.2
    set mytics 2
    set yrange [0:1.5]
}

if ("${range}" ne "") {
    set yrange ${range}
}

set grid xtics mxtics lt 1 lc rgb "gray90" lw 1, lt 1 lc rgb "gray90" lw 1
set key noenhanced left top opaque horizontal

#-------------------------------------------------------------------------------
# Styles
#-------------------------------------------------------------------------------

qm_index(algo) = \
  algo eq "built_in"          ? 1 : algo eq "built_in_distance" ? 2 : \
  algo eq "minverse"          ? 3 :                                   \
  algo eq "mshift"            ? 4 : algo eq "mshift_promoted"   ? 5 : \
  algo eq "mcomp"             ? 6 : algo eq "mcomp_promoted"    ? 7 : \
  algo eq "new_algo"          ? 8 : 0

qm_style = "linespoints"
do for [i=1:8] { set linetype i linewidth 2 pointsize 1.5 pointtype 7 }

if (${use_colours}) {
  set linetype qm_index("built_in")          linecolor rgb "black"      pointtype 2
  set linetype qm_index("built_in_distance") linecolor rgb "black"      pointtype 3
  set linetype qm_index("minverse")          linecolor rgb "red"        pointtype 7
  set linetype qm_index("mcomp")             linecolor rgb "dark-green" pointtype 2
  set linetype qm_index("mcomp_promoted")    linecolor rgb "dark-green" pointtype 3
  set linetype qm_index("mshift")            linecolor rgb "orange"     pointtype 2
  set linetype qm_index("mshift_promoted")   linecolor rgb "orange"     pointtype 3
  set linetype qm_index("new_algo")          linecolor rgb "purple"     pointtype 2
}
else {
  set linetype qm_index("built_in")          linecolor rgb "#000000" pointtype 2
  set linetype qm_index("built_in_distance") linecolor rgb "#000000" pointtype 3
  set linetype qm_index("minverse")          linecolor rgb "#303030" pointtype 7
  set linetype qm_index("mcomp")             linecolor rgb "#606060" pointtype 2
  set linetype qm_index("mcomp_promoted")    linecolor rgb "#606060" pointtype 3
  set linetype qm_index("mshift")            linecolor rgb "#909090" pointtype 2
  set linetype qm_index("mshift_promoted")   linecolor rgb "#909090" pointtype 3
  set linetype qm_index("new_algo")          linecolor rgb "#c0c0c0" pointtype 2
}

#-------------------------------------------------------------------------------
# Plot
#-------------------------------------------------------------------------------

if (${to_file}) {
  set terminal png
  set output "${out_file}"
}

plot for [ algo in "${algos}" ] "<" . awk(algo) \
  using 1:((\$2 - ${noop})/baseline[\$1]) \
  title algo linestyle qm_index(algo) with @qm_style

EOF
