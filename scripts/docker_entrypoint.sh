#!/bin/bash

#force failure on first error
set -e

info_fn() {
	uname -a
	echo "CC=$CC, CXX=$CXX"
}

build_fn() {
	echo "build stage"
	cmake \
		-B /build \
		-S /glpp  \
		-DCMAKE_CXX_COMPILER="$CXX" \
		-DCMAKE_C_COMPILER="$CC"
		
    cmake --build /build
}

test_fn() {
	echo "test stage"
	cmake --build /build --target test
}

install_test() {
	echo "install stage"
	cmake --install /build --prefix $1
	echo "post install test stage"
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
	if [[ "$2" == "WITH_PREFIX" ]]; then
		cmake ..  -DCMAKE_CXX_COMPILER="$CXX" -DCMAKE_C_COMPILER="$CC" -DCMAKE_IGNORE_PATH=/glpp -DCMAKE_PREFIX_PATH=$1
	else
		cmake ..  -DCMAKE_CXX_COMPILER="$CXX" -DCMAKE_C_COMPILER="$CC" -DCMAKE_IGNORE_PATH=/glpp
	fi
	make -j
	ctest --output-on-failure
	rm -r `find $1 -type d -name "*glpp*"`
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
		chmod a+rwx /glpp/scripts
		chmod a+rwx /output
		cd /glpp/scripts
		sudo -u nobody makepkg --clean -f
		cp *.pkg.tar.zst /output
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
	install_test /usr
	install_test /usr/local
	install_test /some/crazy/install/path WITH_PREFIX
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
