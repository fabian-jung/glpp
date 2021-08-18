#!/bin/bash

# This script will be run by a vanilla arch/debian/ubuntu container to
# verify the installation from a glpp package.
# It requires the glpp directory mounted to /glpp
# It requires the glpp package containing folder mounted to /package

# Can be execuded with the following command in the glpp source directory:
# docker run -v "$(pwd)":/glpp -it archlinux /glpp/scripts/package_test.sh


docker run -v "$(pwd)":/glpp -it archlinux /glpp/scripts/package_test.sh
#force failure on first error
set -e

pacman --noconfirm -Syu
pacman --noconfirm -S xorg-server-xvfb mesa libva-mesa-driver
pacman --noconfirm -S blender

#init virtual screen
Xvfb :0 &
export DISPLAY=:0

pacman --noconfirm -U /package/glpp-*.pkg.tar.zst

mkdir /test
cp -r /glpp/test /test

cd /test
cat > CMakeLists.txt << EOF
	cmake_minimum_required(VERSION 3.16)
	project(glpp-tests)

	find_package(glpp REQUIRED)

	set(enable_unit_test TRUE)

	add_subdirectory(test)
EOF
mkdir build
cd build
cmake ..
make -j
ctest --output-on-failure