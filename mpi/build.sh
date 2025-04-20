#!/bin/bash

CC="mpicc"
if [[ -n $3 ]]; then
	SRC="$3.c"
else
	echo "usage: $0 run [num_of_proc] [file_name]"
	exit 1
fi
TARGET="build/erm.out"

if [[ $1 == "run" ]]; then
	if [[ ! -d build ]]; then
		mkdir build
	fi
	$CC -o $TARGET $SRC 
	# either provide num of processors or use all the avaialable.
	if [[ -n $2 ]]; then
		mpirun -np $2 $TARGET
	else
		np=$(nproc)
		mpirun -np $np $TARGET
	fi
else
	echo "usage: $0 run [num_of_proc] [file_name]"
fi
