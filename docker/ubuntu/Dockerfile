FROM ubuntu:rolling
ENV DEBIAN_FRONTEND=noninteractivefile:///home/fabian/projects/glpp/docker/ubuntu/Dockerfile
ENV TZ=Europe/Berlin

RUN apt-get update -y && apt-get upgrade -y && apt-get dist-upgrade -y
RUN apt-get install -y apt-utils

# Compiler
RUN apt-get install -y gcc-11 g++-11
RUN apt-get install -y clang

# Virtual X11
RUN apt-get install -y xvfb libglapi-mesa

# Install dependencies
RUN apt-get install -y libglfw3-dev libglew-dev libglm-dev catch2 libassimp-dev libfreetype-dev libboost-dev pkgconf
RUN apt-get install -y blender
RUN apt-get install -y cmake make ninja-build git

# Documentation tools
RUN apt-get install -y python3 pip doxygen
RUN pip3 install poxy

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 50
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-11 50

RUN mkdir /glpp
RUN mkdir /build
RUN mkdir /test
RUN mkdir -p /install/path

WORKDIR /

ENTRYPOINT ["/bin/bash", "/glpp/scripts/docker_entrypoint.sh"]
CMD ["info"]
