cmake_minimum_required(VERSION 3.2)
cmake_policy(SET CMP0054 NEW)

project (aff3ct-server)

# Enable C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify bin path
set (EXECUTABLE_OUTPUT_PATH bin/)

get_filename_component(TOP_LEVEL_PATH ../ ABSOLUTE)
set(PROTO_DIR ${TOP_LEVEL_PATH}/proto)

# Link with the "Threads library (required to link with AFF3CT after)
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)

message ("PROTO_DIR = ${PROTO_DIR}/")

# Create the executable from sources
#add_executable(aff3ct-server ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp ${PROTO_SRCS} ${PROTO_HDRS})
add_executable(aff3ct-server ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp )

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${PROTO_DIR}/aff3ct.proto)

# Print path to generated files
message ("PROTO_SRCS = ${PROTO_SRCS}")
message ("PROTO_HDRS = ${PROTO_HDRS}")

#Link with zeromq
find_package(PkgConfig)
pkg_check_modules(PC_ZeroMQ QUIET zmq)
## use the hint from above to find where 'zmq.hpp' is located
find_path(ZeroMQ_INCLUDE_DIR
        NAMES zmq.hpp
        PATHS ${PC_ZeroMQ_INCLUDE_DIRS}
        )
## use the hint from about to find the location of libzmq
find_library(ZeroMQ_LIBRARY
        NAMES zmq
        PATHS ${PC_ZeroMQ_LIBRARY_DIRS}
        )



# Link with AFF3CT
set (AFF3CT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")
find_package(AFF3CT)
target_include_directories(aff3ct-server PUBLIC ${ZeroMQ_INCLUDE_DIR} ${PROTOBUF_INCLUDE_DIRS})
target_link_libraries(aff3ct-server PRIVATE aff3ct::aff3ct-static-lib PUBLIC ${ZeroMQ_LIBRARY} ${PROTOBUF_LIBRARIES})
