#!/bin/sh

# From benchmark game:
#
#  java -server -XX:+TieredCompilation -XX:+AggressiveOpts -Xbootclasspath/a:/usr/local/src/scala-2.10.2/lib/scala-library.jar:/usr/local/src/scala-2.10.2/lib/akka-actors.jar:/usr/local/src/scala-2.10.2/lib/typesafe-config.jar threadring 500000
#

N="${1}"

java -server -XX:+TieredCompilation -XX:+AggressiveOpts -Xbootclasspath/a:/opt/scala/lib/scala-library.jar tokenring $N
