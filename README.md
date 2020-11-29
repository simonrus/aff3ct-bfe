# Introduction (Motivation)
The project allows 3rd party applications to remotely encode/decode messages using different FEC codecs from AFF3CT library

The messages and codec settings are transmitted using RPC over flatbuffers

The project can be used to compare FEC codec implementations with a well-known reference implementation

# Current state
Still in development phase! 

Rename from MISSFEC -> aff3ct-bfe (Aff3ct backend-fronend)

# Components
This project provides backend and sample python-frontend for the Fast Forward Error Correction toolbox. 

It has the following parts:
- Aff3ct-server is a backend. It runs as a stand-alone server, executes RPC commands and allows to encode/decode messages using FEC.
- RPC Protocol describes synchronous mechanism and messages used to 
    -- configure aff3ct-library server
    -- push/pull data (vectors/matrices) from frontend to backend 
    -- execute operations on the vectors/matrix using aff3ct-library
- Aff3ct-client is a simple frontend, written in Python. It provides an interactive command shell to control remote aff3ct server

# Repo management
## 1.1 Sync procedure with main repo

	$ git pull sync master
	$ git push -u origin master

# Usefull links
https://www.uni-kl.de/channel-codes/channel-codes-database/more-ldpc-codes
http://www.inference.org.uk/mackay/codes/alist.html

# Install everything
	$ sudo apt-get install pkg-config
## ZeroMQ
	$ sudo apt-get install libzmq3-dev
## cxxtest(Optional)
    $ sudo apt-get install cxxtest
## Python3 (anaconda way)
	$ conda create --name py3 python=3.5
	$ conda activate py3
	$ pip install pyzmq
	$ pip install ipython
	$ pip install cmd2
	$ pip install tqdm ConfigParser
## flatbuffers (local install)
	$ git submodule update --init --recursive
    $ cd 3rdparty/flatbuffers
    $ cmake . && make -j 4 all

## aff3ct-bfe  
	$ git clone git@github.com:simonrus/aff3ct-bfe.git
	$ cd afaff3ct-bfee/lib
	$ git submodule update --init --recursive
## Select compiler (Optional)
Aff3ct uses structures inside class to declare i.e. parameters. The GCC toolchain and gdb are not happy with this and ofter cannot find symbols.

For debugging puposes it is better to use CLANG
    $ export CC=/usr/bin/clang-6.0
    $ export CXX=/usr/bin/clang++-6.0 
## aff3ct library
	$ cd aff3ct
	$ mkdir build
	$ cd build
	$ cmake .. -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-funroll-loops -march=native" -DAFF3CT_COMPILE_EXE="OFF" -DAFF3CT_COMPILE_STATIC_LIB="ON" -DAFF3CT_COMPILE_SHARED_LIB="ON"
	or for the Debug version
	$ cmake .. -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS=" -march=native -ggdb" -DAFF3CT_COMPILE_EXE="OFF" -DAFF3CT_COMPILE_STATIC_LIB="ON" -DAFF3CT_COMPILE_SHARED_LIB="ON"
	then
	$ make -j8
	$ cd ../../../aff3ct-server

## aff3ct binaries (optional) for compare own LDPC decoder implementation
	$ cd aff3ct
	$ mkdir build_exe
	$ cd build_exe
    $ cmake .. -G"Unix Makefiles" -DCMAKE_CXX_COMPILER="g++" -DCMAKE_BUILD_TYPE="Release" -DCMAKE_CXX_FLAGS="-funroll-loops -march=native"
    $ make -j8

## post steps
follow README.RE

# Running Samples
## Server
	$ aff3ct-server
## Interactive Client
	$ source ~/bin/anaconda.sh
	$ conda activate py3
