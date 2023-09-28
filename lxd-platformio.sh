#!/bin/sh


# start at https://documentation.ubuntu.com/lxd/en/latest/tutorial/first_steps

sudo snap refresh lxd
lxc launch images:debian/12 bkw
lxc exec bkw -- apt-get update
lxc exec bkw -- apt-get install -y --no-install-recommends ca-certificates curl
lxc exec bkw -- apt-get install -y pipx
lxc exec bkw -- adduser --system --uid 999 --ingroup dialout --shell /bin/bash --home /home/bee bee
lxc exec bkw --user 999 -- pipx install platformio
lxc exec bkw --user 999 -- pipx ensurepath
lxc exec bkw --user 999 --cwd /home/bee -- mkdir blinky
lxc exec bkw --user 999 --cwd /home/bee --env PATH=/home/bee/.local/bin:$PATH -- pio project init -d /home/bee/blinky --board bee_s3
lxc file push main.cpp bkw/home/bee/blinky/src/main.cpp
lxc exec bkw --user 999 --cwd /home/bee --env PATH=/home/bee/.local/bin:$PATH -- pio pkg install -d /home/bee/blinky --library "strid3r21/BeeS3@^1.0.8"
lxc exec bkw --user 999 --cwd /home/bee --env PATH=/home/bee/.local/bin:$PATH -- pio run -d /home/bee/blinky


# manual step: plug in Bee S3 (TODO ttyACM0 is not available to container)
# 1. hold boot btn then click reset btn then release boot btn to start bootloader mode
# 2. run `pio run -t upload -d $HOME/blinky`
# 3. click reset btn to exit bootloader mode
# 4. run `screen /dev/ttyACM0 9600`
##lxc exec bkw --user 999 --cwd /home/bee --env PATH=/home/bee/.local/bin:$PATH -- pio run -d /home/bee/blinky -t upload

# additional cleaning
##pio run -t clean
##pio upgrade
##pio system prune
##pio pkg update

