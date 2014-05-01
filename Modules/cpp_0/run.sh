#!/bin/bash
./compile.sh 1>&2 && ./bot
echo "Exited with status $?." 1>&2
