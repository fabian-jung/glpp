FROM ubuntu:rolling
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Berlin

RUN apt-get update -y && apt-get upgrade -y && apt-get dist-upgrade -y
RUN apt-get install -y apt-utils

# Compiler
RUN apt-get install -y gcc-11 g++-11
RUN apt-get install -y clang
RUN apt-get install -y git

# Virtual X11
RUN apt-get install -y libglapi-mesa

# Install dependencies
RUN apt-get install -y libglfw3-dev libglew-dev libglm-dev catch2 libassimp-dev libfreetype-dev libfmt-dev libboost-dev pkgconf
RUN apt-get install -y blender
RUN apt-get install -y cmake make ninja-build git

# Install boost pfr, since ubuntu ships with boost 1.74 and pfr got added  with 1.75
RUN mkdir /temp && cd /temp && git clone https://github.com/boostorg/pfr.git && cp -r pfr/include/boost/* /usr/include/boost && cd / && rm -rf /temp

# Documentation tools
RUN apt-get install -y python3 pip doxygen
RUN pip3 install poxy

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 50
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-11 50

RUN mkdir /glpp
RUN mkdir /build
RUN mkdir /test
RUN mkdir /output
RUN mkdir -p /install/path

WORKDIR /

ENTRYPOINT ["/bin/bash", "/glpp/scripts/docker_entrypoint.sh"]
CMD ["info"]
