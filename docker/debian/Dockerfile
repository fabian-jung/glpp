FROM debian:unstable
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Berlin

RUN apt-get update -y && apt-get upgrade -y && apt-get dist-upgrade -y
RUN apt-get install -y apt-utils
RUN apt-get install -y gcc clang
RUN apt-get install -y libglapi-mesa libglfw3-dev libglew-dev libglm-dev catch2 libassimp-dev libfreetype-dev pkgconf
RUN apt-get install -y cmake make ninja-build git blender

RUN mkdir /glpp
RUN mkdir /build

WORKDIR /

ENTRYPOINT ["/bin/bash", "/glpp/scripts/docker_entrypoint.sh"]
CMD ["info"]
