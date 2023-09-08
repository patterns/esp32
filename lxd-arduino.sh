#!/bin/sh


# start at https://documentation.ubuntu.com/lxd/en/latest/tutorial/first_steps

sudo snap refresh lxd
lxc launch images:debian/12 bkw
lxc exec bkw -- apt-get update
lxc exec bkw -- apt-get install -y --no-install-recommends ca-certificates curl
# note: don't use the install.sh script because it runs outside the container
lxc exec bkw -- curl -sL -o /tmp/arduinocli.tgz https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Linux_ARM64.tar.gz
lxc exec bkw -- tar xzf /tmp/arduinocli.tgz -C /usr/local/bin
lxc exec bkw -- arduino-cli config init
lxc exec bkw -- arduino-cli sketch new MyFirstSketch
#lxc file push MyFirstSketch.ino bkw/root/MyFirstSketch/MyFirstSketch.ino
lxc exec bkw -- arduino-cli core update-index
# manual step: plug in esp32-s3 (TODO ttyACM0 is not available to container)
lxc exec bkw -- arduino-cli board list


