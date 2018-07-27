#!/bin/bash
#
export LD_LIBRARY_PATH=/home/dirk/Projects/Current/Linux/HC12Radio/build:$LD_LIBRARY_PATH

sudo LD_LIBRARY_PATH=~/Projects/HC12Radio/build:$LD_LIBRARY_PATH ./hc12Test -c /dev/ttyS0
