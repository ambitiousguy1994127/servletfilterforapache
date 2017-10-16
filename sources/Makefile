#Makefile

CXXFLAGS = -std=c++11

all: module install
	
module:
	scons

install:
	sudo apxs -i -n idfc mod_idfc.so
	
clean:
	scons -c
	
