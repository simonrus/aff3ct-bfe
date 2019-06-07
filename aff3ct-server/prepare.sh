mkdir cmake && mkdir cmake/Modules
cp ../lib/aff3ct/build/lib/cmake/aff3ct-*/* cmake/Modules
mkdir build
cd build
cmake .. -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-funroll-loops -march=native"
make

