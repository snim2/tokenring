#!/bin/sh

N="${1}"

java -server -XX:+TieredCompilation -XX:+AggressiveOpts  -cp .:/usr/share/java/clojure.jar: tokenring $N
