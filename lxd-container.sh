#!/bin/sh


# start at https://documentation.ubuntu.com/lxd/en/latest/tutorial/first_steps

sudo snap refresh lxd
lxc launch images:debian/12 bookworm
lxc exec bookworm -- apt-get update
lxc exec bookworm -- apt-get install -y --no-install-recommends git ca-certificates curl \
    bison flex gettext texinfo libncurses5-dev libncursesw5-dev \
    gperf automake libtool pkg-config build-essential \
    libgmp-dev libmpc-dev libmpfr-dev libisl-dev binutils-dev libelf-dev \
    libexpat-dev picocom u-boot-tools util-linux \
    kconfig-frontends genromfs

lxc exec bookworm -- mkdir /tmp/nuttxspace
lxc exec bookworm -- curl -sL https://github.com/apache/nuttx/tarball/master \
    -o /tmp/nuttx.tgz
lxc exec bookworm -- curl -sL https://github.com/apache/nuttx-apps/tarball/master \
    -o /tmp/apps.tgz
lxc exec bookworm -- tar xzf /tmp/nuttx.tgz -C /tmp/nuttxspace --one-top-level=nuttx \
    --strip-components 1 
lxc exec bookworm -- tar xzf /tmp/apps.tgz -C /tmp/nuttxspace --one-top-level=apps \
    --strip-components 1 
lxc exec bookworm -- curl -sL https://github.com/espressif/crosstool-NG/releases/download/esp-12.2.0_20230208/xtensa-esp32s3-elf-12.2.0_20230208-aarch64-linux-gnu.tar.xz \
    | tar x -Jv -C /tmp -f -
lxc exec bookworm -- mv /tmp/xtensa-esp32s3-elf /opt/xtensa
lxc exec bookworm -- mkdir /tmp/nuttxspace/esp-bins
lxc exec bookworm -- curl -sL https://github.com/espressif/esp-nuttx-bootloader/releases/download/latest/partition-table-esp32s3.bin \
    -o /tmp/nuttxspace/esp-bins/partition-table-esp32s3.bin
lxc exec bookworm -- curl -sL https://github.com/espressif/esp-nuttx-bootloader/releases/download/latest/bootloader-esp32s3.bin \
    -o /tmp/nuttxspace/esp-bins/bootloader-esp32s3.bin


lxc exec bookworm -- apt-get install pipx
lxc exec bookworm -- pipx install esptool
lxc exec bookworm -- pipx install pyserial
lxc exec bookworm -- pipx ensurepath
##lxc exec bookworm -- export PATH=$PATH:/opt/xtensa/bin:/$HOME/.local/bin
lxc exec bookworm -- cd /tmp/nuttxspace/nuttx && \
    ./tools/configure.sh -l esp32s3-devkit:nsh && make



lxc exec bookworm -- curl -sL https://ziglang.org/builds/zig-linux-aarch64-0.12.0-dev.244+f4c9e19bc.tar.xz \
    | tar x -Jv -C $HOME/.local -f -

lxc exec bookworm -- ln -s $HOME/.local/zig-linux-aarch64-0.12.0-dev.244+f4c9e19bc/zig $HOME/.local/bin/zig
lxc exec bookworm -- zig build-obj -target=riscv32-freestanding-none \
    -mcpu=baseline_rv32-d  hello.zig

#TODO esp32s3 datasheet says RISCV and single precision FPU,
#     what are its other attributes?
# https://zig.news/lupyuen/zig-on-risc-v-bl602-quick-peek-with-apache-nuttx-rtos-3apd
 
