#!/bin/sh

N="${!}"

mpirun -n $N ./tokenring
