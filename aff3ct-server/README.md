# Introduction

Aff3ct-server is a backend for aff3ct-library. It runs as a stand-alone server, 
executes RPC commands and allows to remotely control Fast Forward Error 
Correction Toolbox (https://aff3ct.github.io/)

The transport for RPC is ZeroMQ and protobuf

# How to compile aff3server

Make sure to have done the instructions from the `README.md` file at the root of this repository before doing this.

Copy the cmake configuration files from the AFF3CT build

	$ mkdir cmake && mkdir cmake/Modules
	$ cp ../lib/aff3ct/build/lib/cmake/aff3ct-*/* cmake/Modules
	$ cp ../3rdparty/flatbuffers/CMake/*.cmake cmake/Modules    

Compile the code on Linux/MacOS/MinGW:

	$ mkdir build
	$ cd build
	$ cmake .. -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-funroll-loops -march=native"
    or 
	$ cmake .. -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-march=native -ggdb"
	$ make

Compile the code on Windows (Visual Studio project)

	$ mkdir build
	$ cd build
	$ cmake .. -G"Visual Studio 15 2017 Win64" -DCMAKE_CXX_FLAGS="-D_SCL_SECURE_NO_WARNINGS /EHsc"
	$ devenv /build Release my_project.sln

The source code of this mini project is in `src/main.cpp`.
The compiled binary is in `build/bin/my_project`.

Have fun :-)!
