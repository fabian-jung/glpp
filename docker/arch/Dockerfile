FROM archlinux

RUN pacman --noconfirm -Syu

# needed for makepkg for packaging
RUN pacman --noconfirm -S sudo fakeroot 
RUN pacman --noconfirm -S gcc clang

# Virtual xserver and mesa is needed for offscreen testing in CI
RUN pacman --noconfirm -S mesa libva-mesa-driver

# Required dependencies
RUN pacman --noconfirm -S glfw glew glm catch2 assimp freetype2 pkgconf boost

# Required binaries
RUN pacman --noconfirm -S cmake make ninja git

# blender is needed to translate the testing meshes in CI
RUN pacman --noconfirm -S blender

RUN mkdir /glpp
RUN mkdir /build
RUN mkdir /test
RUN mkdir /output
RUN mkdir -p /install/path

WORKDIR /

ENTRYPOINT ["/bin/bash", "/glpp/scripts/docker_entrypoint.sh"]
CMD ["info"]
