#!/bin/sh

N="${1}"

erl -smp disable -noshell -run +t 8192 +ec +K true +P 50000000 +hmbs 1 +hms 4 +sss 4 tokenring main $N
