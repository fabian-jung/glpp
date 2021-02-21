#!/bin/bash

info_fn() {
	uname -a
}

build_fn() {
	echo "build stage"
	mkdir build
	cd build
	cmake ..
	make -j
}

test_fn() {
	ctest
}

if [[ "$1" == "info" ]]; then
	info_fn
elif [[ "$1" == "build" ]]; then
	build_fn
elif [[ "$1" == "test" ]]; then
	build_fn
	test_fn
elif [[ "$1" == "bash" ]]; then
	/bin/bash
else
	echo "No command provided"
	exit 1
fi
