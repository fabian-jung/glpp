#!/bin/bash

Xvfb :0 &
export DISPLAY=:0

info_fn() {
	uname -a
}

build_fn() {
	echo "build stage"
	cd /build
	cmake /glpp
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
