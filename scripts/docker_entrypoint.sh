#!/bin/bash

#init virtual screen
export DISPLAY=:0

#force failure on first error
set -e

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

build_doc() {
	echo "build documentation"
	cd /build
	cmake /glpp -DCMAKE_CXX_COMPILER="$CXX" -DCMAKE_C_COMPILER="$CC"
	make doc
}

package() {
	source /etc/os-release
	if [[ "$ID" == "arch" ]]; then
		# docker run \
		# --mount src="$(pwd)",target=/glpp,type=bind \
		# --mount src="$(pwd)/package",target=/output,type=bind 
		# ghcr.io/fabian-jung/glpp/build-arch package
		echo "packaging on arch"
		cd /glpp/scripts
		chmod a+rwx /build
		sudo -u nobody makepkg BUILDDIR=/build PKGDEST=/build SRCDEST=/build --clean -f
		cp /build/*.pkg.tar.zst /output
	elif [[ "$ID" == "debian" ]]; then
		# docker run \
		# --mount src="$(pwd)",target=/glpp,type=bind \
		# --mount src="$(pwd)/package",target=/output,type=bind 
		# ghcr.io/fabian-jung/glpp/build-debian package
		echo "packaging on debian"
	elif [[ "$ID" == "ubuntu" ]]; then
		# docker run \
		# --mount src="$(pwd)",target=/glpp,type=bind \
		# --mount src="$(pwd)/package",target=/output,type=bind 
		# ghcr.io/fabian-jung/glpp/build-ubuntu package
		echo "packaging on ubuntu"
		build_fn
		test_fn
		make package
		cp *.deb /output
	fi
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
elif [[ "$1" == "doc" ]]; then
	build_doc
elif [[ "$1" == "package" ]]; then
	package
elif [[ "$1" == "bash" ]]; then
	/bin/bash
else
	echo "No command provided"
	exit 1
fi
