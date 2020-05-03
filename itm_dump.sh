#!/bin/bash

#/Users/whoisvolos/repos/embedded/stm32_cmake_boot/itm.fifo

openocd \
	-f interface/stlink-v2.cfg \
	-f target/stm32f4x.cfg \
	-c "tpiu config internal - uart off 84000000" \
	-c "itm ports on"
