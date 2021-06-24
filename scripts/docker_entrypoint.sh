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

install_fn() {
	make install
}

post_install_test_fn() {
	cd /test
	cp -r /glpp/test .

	cat > CMakeLists.txt << EOF
	    cmake_minimum_required(VERSION 3.16)
		project(glpp-tests)

		find_package(Catch2 REQUIRED)
		include(CTest NO_POLICY_SCOPE)
		include(Catch NO_POLICY_SCOPE)
		include(ParseAndAddCatchTests)

		find_package(glpp REQUIRED)

		set(enable_unit_test TRUE)

		add_subdirectory(test)
EOF
	rm -rf build
	mkdir build

	cd build
	cmake ..
	make -j
	ctest
}

if [[ "$1" == "info" ]]; then
	info_fn
elif [[ "$1" == "build" ]]; then
	build_fn
elif [[ "$1" == "test" ]]; then
	build_fn
	test_fn
	install_fn
	post_install_test_fn

elif [[ "$1" == "bash" ]]; then
	/bin/bash
else
	echo "No command provided"
	exit 1
fi
