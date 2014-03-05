#!/bin/sh

N="${1}"

java -server -XX:+TieredCompilation -XX:+AggressiveOpts Tokenring $N

