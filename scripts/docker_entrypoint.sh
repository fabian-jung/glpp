#!/bin/bash

Xvfb :0 &
export DISPLAY=:0

info_fn() {
	uname -a
	echo "CC=$CC, CXX=$CXX"
}

build_fn() {
	echo "build stage"
	cd /build
	cmake /glpp -DCMAKE_CXX_COMPILER="$CXX" -DCMAKE_C_COMPILER="$CC"
	make -j
}

test_fn() {
	ctest --output-on-failure
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
	cmake ..  -DCMAKE_CXX_COMPILER="$CXX" -DCMAKE_C_COMPILER="$CC"
	make -j
	ctest --output-on-failure
}

if [[ "$2" == "clang" ]]; then
	CC="clang";
	CXX="clang++";
else
	CC="gcc";
	CXX="g++";
fi


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
