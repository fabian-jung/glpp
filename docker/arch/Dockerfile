FROM archlinux

RUN pacman --noconfirm -Syu
RUN pacman --noconfirm -S gcc clang
RUN pacman --noconfirm -S mesa glfw glew glm catch2 assimp freetype2 pkgconf
RUN pacman --noconfirm -S cmake make ninja git blender
RUN mkdir /glpp
RUN mkdir /build

WORKDIR /

ENTRYPOINT ["/bin/bash", "/glpp/scripts/docker_entrypoint.sh"]
CMD ["info"]
