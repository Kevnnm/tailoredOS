# Container for clean build environment
#
# docker build <dockerfile> -t <image name>
# Linux or MacOS : docker run --rm -it -v $(pwd):/root/env <image name>
# Windows        : docker run --rm -it -v %cd%:/root/env <image name>

FROM randomdude/gcc-cross-x86_64-elf

RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get upgrade -y nasm
RUN apt-get upgrade -y xorriso # grub dependency
RUN apt-get upgrade -y grub-pc-bin
RUN apt-get upgrade -y grub-common

VOLUME /root/env
WORKDIR /root/env
