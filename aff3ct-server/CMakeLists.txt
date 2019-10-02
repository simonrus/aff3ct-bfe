cmake_minimum_required(VERSION 3.2)
cmake_policy(SET CMP0054 NEW)

project (aff3ct-server)

# Enable C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -no-pie")
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

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_DIR}/aff3ct.proto)

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

#add logging module
set(LOGURU_DIR ${TOP_LEVEL_PATH}/3rdparty/loguru)
set(LOGURU_INCLUDE_DIRS ${LOGURU_DIR})
set(LOGURU_SRCS ${LOGURU_DIR}/loguru.cpp)

set(AFF3CT_ADDONS_SRCS
${CMAKE_CURRENT_SOURCE_DIR}/src/aff3ct-addons/Source_memory.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/aff3ct-addons/factory/OnlyCodec.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/aff3ct-addons/launcher/CodecRun.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/aff3ct-addons/launcher/OnlyCodec.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/aff3ct-addons/simulation/CodecRun.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/aff3ct-addons/simulation/OnlyCodec.cpp
)
    
#set main sources
set(MAIN_SRCS
${CMAKE_CURRENT_SOURCE_DIR}/src/Facade_Codec.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/aff3ct-errc.cpp
)

#add_executable(aff3ct-server ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp ${MAIN_SRCS} ${AFF3CT_ADDONS_SRCS} ${PROTO_SRCS} ${PROTO_HDRS} ${LOGURU_SRCS})
add_executable(aff3ct-test ${CMAKE_CURRENT_SOURCE_DIR}/src/test.cpp ${MAIN_SRCS} ${AFF3CT_ADDONS_SRCS} ${PROTO_SRCS} ${PROTO_HDRS} ${LOGURU_SRCS})

# Link with AFF3CT
set (AFF3CT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")
find_package(AFF3CT)

set(LOCAL_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

#target_include_directories(aff3ct-server PUBLIC ${LOCAL_INCLUDE_DIRS} ${ZeroMQ_INCLUDE_DIR} ${PROTOBUF_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR} ${LOGURU_INCLUDE_DIRS})
target_include_directories(aff3ct-test PUBLIC ${LOCAL_INCLUDE_DIRS} ${ZeroMQ_INCLUDE_DIR} ${PROTOBUF_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR} ${LOGURU_INCLUDE_DIRS})

if(NOT WIN32)
    set(DL_LIBRARY dl)
endif()

#target_link_libraries(aff3ct-server PRIVATE aff3ct::aff3ct-static-lib PUBLIC ${ZeroMQ_LIBRARY} ${PROTOBUF_LIBRARIES} ${DL_LIBRARY})
target_link_libraries(aff3ct-test PRIVATE aff3ct::aff3ct-static-lib PUBLIC ${ZeroMQ_LIBRARY} ${PROTOBUF_LIBRARIES} ${DL_LIBRARY})
