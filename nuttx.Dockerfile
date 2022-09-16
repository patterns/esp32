
##FROM ubuntu:jammy 
FROM python:3.10.7-bullseye 
RUN apt-get update && apt-get install -y --no-install-recommends git ca-certificates curl \
    bison flex gettext texinfo libncurses5-dev libncursesw5-dev \
    gperf automake libtool pkg-config build-essential gperf \
    libgmp-dev libmpc-dev libmpfr-dev libisl-dev binutils-dev libelf-dev \
    libexpat-dev picocom u-boot-tools util-linux \
    kconfig-frontends 

ARG USERNAME=user-name-goes-here
ARG USER_UID=1000
ARG USER_GID=$USER_UID

# Create the user
RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
    #
    # [Optional] Add sudo support. Omit if you don't need to install software after connecting.
    && apt-get update \
    && apt-get install -y sudo \
    && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

RUN adduser $USERNAME dialout
USER $USERNAME

ENV PATH "$PATH:/opt/xtensa/bin:/home/${USERNAME}/.local/bin"
ENV WDIR "/home/${USERNAME}/nuttxspace"
#WORKDIR /home/${USERNAME}/nuttxspace
RUN mkdir -p ${WDIR} && cd ${WDIR}
RUN git clone --depth=1 https://github.com/apache/incubator-nuttx.git ${WDIR}/nuttx
RUN git clone --depth=1 https://github.com/apache/incubator-nuttx-apps.git ${WDIR}/apps
RUN curl -sL https://github.com/espressif/crosstool-NG/releases/download/esp-2022r1/xtensa-esp32s3-elf-gcc11_2_0-esp-2022r1-linux-arm64.tar.xz | tar x -Jv -C ${WDIR} -f -

RUN sudo mv ${WDIR}/xtensa-esp32s3-elf /opt/xtensa ;\
    pip3 install esptool && pip3 install pyserial
RUN mkdir ${WDIR}/esp-bins ;\
    curl -sL https://github.com/espressif/esp-nuttx-bootloader/releases/download/latest/bootloader-esp32.bin -o ${WDIR}/esp-bins/bootloader-esp32.bin ;\
    curl -sL https://github.com/espressif/esp-nuttx-bootloader/releases/download/latest/partition-table-esp32.bin -o ${WDIR}/esp-bins/partition-table-esp32.bin 

# cd ${WDIR}/nuttx && ./tools/configure.sh -L |more
RUN cd ${WDIR}/nuttx && ./tools/configure.sh -l esp32s3-devkit:nsh
#   && make


