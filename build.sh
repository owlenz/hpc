#!/bin/bash

CC="mpicc"
SRC="mpi.c"
TARGET="build/erm.out"

if [[ $1 == "run" ]]; then
	if [[ ! -d build ]]; then
		mkdir build
	fi
	$CC -o $TARGET $SRC 
	if [[ -n $2 ]]; then
		mpirun -np $2 $TARGET
	else
		np=$(nproc)
		mpirun -np $np $TARGET
	fi
else
	echo "usage: $0 run [num_of_proc]"
fi
