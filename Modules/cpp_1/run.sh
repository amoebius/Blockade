#!/bin/bash
./compile.sh "$1" 1>&2 && ./bot
echo "" 1>&2
echo "Exited with status $?." 1>&2
