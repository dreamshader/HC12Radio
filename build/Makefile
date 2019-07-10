#################################################################################
# Makefile for hc12Radio library and example
#
#################################################################################
# This Makefile is part of my hc12Radio project
#	git://github.com/dreamshader/hc12Radio.git
#
#  Copyright (C) 2018 Dreamshader (aka Dirk Schanz)
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#  
#      http://www.apache.org/licenses/LICENSE-2.0
#  
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
#################################################################################
# Note: you may copy the library to every location you want.
#       Hold in mind that, in case of you use the shared library, 
#       you have to set your LD_LIBRARY_PATH environment variable
#       to point to that location 
#       ( e.g.  export LD_LIBRARY_PATH=~/mylibs/:LD_LIBRARY_PATH} ).
#       In addition you have to tell the compiler where to find the
#       library using the -I and -L arguments (e.g. -L./ -I./ ) and
#       what's the name of the lib with the -l argument( -l hc12Radio ).
#
#################################################################################
CXX = g++
CC  = gcc
AR  = ar
#
CCDEBUG  = -g
CXXDEBUG = -g
#
CXXFLAGS = -Wall -DHC12SERIAL
CXXLIBSOFLAGS = -fPIC -shared 
#
SOURCEDIR = ../src
LIBSRC = $(SOURCEDIR)/hc12Radio.cpp
LIBINC = $(SOURCEDIR)/hc12Radio.h
EXAMPLSRC = $(SOURCEDIR)/hc12Test.cpp
EXAMPLNAME = hc12Test
EXAMPLFLAGS = -I. -L ../build -l hc12Radio -l serialConnection
LIBOBJ = hc12Radio.o
SOLIBNAME = libhc12Radio.so
#
UNAME := $(shell uname -m)
#

ifeq ($(UNAME), x86_64)
PIGPIO =
CXXRASPBERRY =
else
PIGPIO = -lpigpio -lrt
CXXRASPBERRY = -DRASPBERRY
endif

all: $(SOLIBNAME) example


$(SOLIBNAME): $(LIBSRC) $(LIBINC)
	$(CXX) $(CXXFLAGS) $(CXXRASPBERRY) $(CXXDEBUG) $(CXXLIBSOFLAGS) -c $(LIBSRC)
	$(CXX) -shared  -Wl,-soname,$(SOLIBNAME) -o $(SOLIBNAME) $(LIBOBJ)

example: $(SOLIBNAME) $(EXAMPLSRC)
	$(CXX) -o $(EXAMPLNAME) $(CXXDEBUG) $(CXXRASPBERRY) $(EXAMPLSRC) $(EXAMPLFLAGS) $(PIGPIO)


install: $(SOLIBNAME)
	sudo install -m 0755 -d                        /usr/local/include
	sudo install -m 0644 $(SOURCEDIR)/hc12Radio.h  /usr/local/include
	sudo install -m 0755 -d                        /usr/local/lib
	sudo install -m 0644 libhc12Radio.so           /usr/local/lib
	$(LDCONFIG)

uninstall:
	sudo rm -f /usr/local/include/hc12Radio.h
	sudo rm -f /usr/local/lib/libhc12Radio.so
	$(LDCONFIG)





clean:
    #$rm *.o *.so

