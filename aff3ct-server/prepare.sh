mkdir cmake && mkdir cmake/Modules
cp ../lib/aff3ct/build/lib/cmake/aff3ct-*/* cmake/Modules
mkdir build
cd build
cmake .. -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-march=native -ggdb"
make

