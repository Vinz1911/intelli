# Intelli v2.0
**Intelli v2.0** is a collection of different projects for the `RoboMaster S1`.
The goal is to get rid of the original `Intelli Controller` from the RoboMaster.
The intelli v2.0 controller is build with a Raspberry Pi 5 and a PiCAN FD HAT, it uses the robomaster c++ library.

## License:
[![License](https://img.shields.io/badge/License-MIT-blue.svg?longCache=true&style=flat)](https://github.com/Vinz1911/intelli/blob/master/LICENSE)

## C++ Version:
[![C++23](https://img.shields.io/badge/C++-23-blue.svg?logo=c%2B%2B&style=flat)](https://isocpp.org)

# Hardware List:
- [Raspberry Pi 5](https://www.raspberrypi.com/products/raspberry-pi-5/)
- [PiCAN FD with GPS uBlox NEO-M8M](https://www.skpang.co.uk/products/pican-fd-with-gpsgnss-can-bus-board-for-raspberry-pi)
- [Adafruit 1.3" OLED Display](https://www.adafruit.com/product/938)
- Custom 3D printed parts

# Linux Kernel:
The intelli v2.0 uses the always latest version of the `Raspberry Pi OS Lite`. For a better performance,
i would recommend to update the kernel to at least `6.12.x` and enable the `PREEMT_RT` patch. This can be done like this:

```shell
# install requirements
sudo apt install git bc bison flex libssl-dev make -y && sudo apt install libncurses5-dev -y && sudo apt install raspberrypi-kernel-headers -y

# clone the linux kernel repo for raspberry pi
git clone --depth 1 --branch rpi-6.12.y https://github.com/raspberrypi/linux

# create configs and enable PREEMT_RT, CONFIG_HZ_1000 and CPU performance governor
make bcm2712_defconfig && make menuconfig

# prepare and compile kernel with all available cores
make prepare && make CFLAGS='-O3 -march=native' -j$(nproc) Image.gz modules dtbs

# copy new kernel to boot location (this overwrites the old kernel)
sudo cp arch/arm64/boot/dts/broadcom/*.dtb /boot/firmware/
sudo cp arch/arm64/boot/dts/overlays/*.dtb* /boot/firmware/overlays/
sudo cp arch/arm64/boot/dts/overlays/README /boot/firmware/overlays/
sudo cp arch/arm64/boot/Image.gz /boot/firmware/kernel_2712.img

# reboot the system
sudo reboot now

# update without kernel upgrade
sudo SKIP_KERNEL=1 PRUNE_MODULES=1 rpi-update rpi-6.12.y
```

# Required Libraries:
To make the intelli v2.0 work there are some required libraries:
- [Sense](https://github.com/Vinz1911/sense) - dual sense controller library
- [Robomaster](https://github.com/Vinz1911/robomaster) - robomaster core library
- [LCDGFX](https://github.com/Vinz1911/lcdgfx) - oled display library
 
```shell
# build and install libraries
mkdir build
cd build/
cmake ..
make && make install
```

# Enable CAN-HAT:
To enable the `MCP2518FD` go to `/boot/firmware/config.txt` and add the following lines:

```shell
dtparam=spi=on
dtparam=i2c_arm=on
dtoverlay=mcp251xfd,spi0-0,interrupt=25
```

# Create Service:
To enable the can interface on startup, copy the `can.service` to `/etc/systemd/system/` and enable it with:

```shell
sudo systemctl daemon-reload
sudo systemctl enable setup-can.service
sudo reboot now
```

# Build Intelli:
Finally build the intelli controller. The controller allows basic remote control options for now:

```shell
# install requirements
sudo apt install git -y && sudo apt install vim -y && sudo apt install build-essential -y && sudo apt install can-utils -y && sudo apt install cmake -y

# build the intelli executable
mkdir build
cd build/
cmake ..
make && make install
```