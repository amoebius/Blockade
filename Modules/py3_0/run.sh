#!/bin/bash
python3 blockade.py ${1%.py}
echo "Exited with status $?." 1>&2
