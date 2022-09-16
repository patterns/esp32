
FROM ubuntu:jammy 

RUN apt-get update && apt-get install -y --no-install-recommends ca-certificates curl \
    git libglib2.0-dev libfdt-dev libpixman-1-dev zlib1g-dev ninja-build \
    libgcrypt-dev git-email build-essential \
 libaio-dev libbluetooth-dev libcapstone-dev libbrlapi-dev libbz2-dev \
 libcap-ng-dev libcurl4-gnutls-dev libgtk-3-dev \
 libibverbs-dev libjpeg8-dev libncurses5-dev libnuma-dev \
 librbd-dev librdmacm-dev \
 libsasl2-dev libsdl2-dev libseccomp-dev libsnappy-dev libssh-dev \
 libvde-dev libvdeplug-dev libvte-2.91-dev libxen-dev liblzo2-dev \
 valgrind xfslibs-dev

RUN git clone --depth=1 https://github.com/espressif/qemu 
RUN mkdir -p qemu/build && cd qemu/build &&\
    ../configure --target-list=xtensa-softmmu \
    --enable-gcrypt \
    --enable-debug --enable-sanitizers \
    --disable-strip --disable-user \
    --disable-capstone --disable-vnc \
    --disable-sdl --disable-gtk

RUN cd qemu && ninja -C build


