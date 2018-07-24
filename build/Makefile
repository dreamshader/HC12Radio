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
LIBSRC = $(SOURCEDIR)/serialConnection.cpp $(SOURCEDIR)/hc12Radio.cpp
EXAMPLSRC = $(SOURCEDIR)/hc12Test.cpp
EXAMPLNAME = hc12Test
EXAMPLFLAGS = -I. -L ../build -l hc12Radio
LIBOBJ = serialConnection.o hc12Radio.o
SOLIBNAME = libhc12Radio.so
STATLIBNAME = libhc12Radio.a
#
#
all: lib-static lib-shared

lib-static:
	$(CXX) $(CXXFLAGS) -c $(LIBSRC)

lib-shared:
	$(CXX) $(CXXFLAGS) $(CXXDEBUG) $(CXXLIBSOFLAGS) -c $(LIBSRC)
	$(CXX) -shared  -Wl,-soname,$(SOLIBNAME) -o $(SOLIBNAME) $(LIBOBJ)

example: lib-static lib-shared
	$(CXX) -o $(EXAMPLNAME) $(CXXDEBUG) $(EXAMPLSRC) $(EXAMPLFLAGS)


#g++ -fPIC -shared myclass.cc -o myclass.so
#g++ class_user.cc -ldl -o class_user
#-Wall
#ar -cvq libctest.a ctest1.o ctest2.o
#ar -t libctest.a
#
#main: libshared.so main.o
    #$(CXX) -o main  main.o -L. -lshared
#
#libshared.so: shared.cpp
    #$(CXX) -fPIC -c shared.cpp -o shared.o
    #$(CXX) -shared  -Wl,-soname,libshared.so -o libshared.so shared.o
#
#clean:
    #$rm *.o *.so

