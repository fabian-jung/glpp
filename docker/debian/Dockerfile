FROM debian:unstable
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Berlin

RUN apt-get update -y && apt-get upgrade -y && apt-get dist-upgrade -y
RUN apt-get install -y apt-utils
RUN apt-get install -y gcc clang git
RUN apt-get install -y libglapi-mesa
RUN apt-get install -y libglfw3-dev libglew-dev libglm-dev catch2 libassimp-dev libfreetype-dev libboost-dev pkgconf
RUN apt-get install -y cmake make ninja-build git blender

# Install boost pfr, since debian ships with boost 1.74 and pfr got added  with 1.75
RUN mkdir /temp && cd /temp && git clone https://github.com/boostorg/pfr.git && cp -r pfr/include/boost/* /usr/include/boost && cd / && rm -rf /temp

RUN mkdir /glpp
RUN mkdir /build
RUN mkdir /test
RUN mkdir /output
RUN mkdir -p /install/path

WORKDIR /

ENTRYPOINT ["/bin/bash", "/glpp/scripts/docker_entrypoint.sh"]
CMD ["info"]
