cmake_minimum_required(VERSION 3.2)
cmake_policy(SET CMP0054 NEW)

project (aff3ct-server)

# Enable C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -no-pie")

# Specify output path
set (EXECUTABLE_OUTPUT_PATH bin/)
set (LIBRARY_OUTPUT_PATH bin/)

# determine missfec path
get_filename_component(TOP_LEVEL_PATH ../ ABSOLUTE)
set(PROTO_DIR ${TOP_LEVEL_PATH}/proto)

# Link with the "Threads library (required to link with AFF3CT after)
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Add flatbuffers 
if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    find_package(Flatbuffers REQUIRED)
    message(STATUS "Flatbuffers dir is ${Flatbuffers_DIR}")
    include("${TOP_LEVEL_PATH}/3rdparty/flatbuffers/CMake/BuildFlatBuffers.cmake" REQUIRED)
    set(FLATBUFFERS_FLATC_EXECUTABLE "flatc")
else()
    set (FLATBUFFERS_INCLUDE_DIRS "${TOP_LEVEL_PATH}/3rdparty/flatbuffers/include")
    set (Flatbuffers_DIR "${TOP_LEVEL_PATH}/3rdparty/flatbuffers/CMake/")
    find_package(Flatbuffers REQUIRED)

    set(FLATBUFFERS_FLATC_EXECUTABLE "${TOP_LEVEL_PATH}/3rdparty/flatbuffers/flatc")
    include("${Flatbuffers_DIR}/BuildFlatBuffers.cmake" REQUIRED)
endif()

set(FLATBUFFERS_GENERATED_HEADERS_DIR        "${PROJECT_BINARY_DIR}/generated")
build_flatbuffers("${PROTO_DIR}/aff3ct-proto.fbs" 
                    "" 
                    "aff3ct_proto_generated_includes"
                    "" 
                    "${FLATBUFFERS_GENERATED_HEADERS_DIR}"
                    "" 
                    "")


#Link with zeromq
find_package(PkgConfig)
##
if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    message(STATUS "APPLE")

    pkg_check_modules(PC_ZeroMQ QUIET zmq)
    
    ## use the hint from above to find where 'zmq.hpp' is located
    find_path(ZeroMQ_INCLUDE_DIR
        NAMES zmq.h
        PATHS ${PC_ZeroMQ_INCLUDE_DIRS}
        )
    
    ## use the hint from about to find the location of libzmq
    find_library(ZeroMQ_LIBRARY
        NAMES zmq
        PATHS ${PC_ZeroMQ_LIBRARY_DIRS}
        )

    set(CPPZeroMQ_DIR ${TOP_LEVEL_PATH}/3rdparty/cppzmq)
    set(CPPZeroMQ_INCLUDE_DIR ${CPPZeroMQ_DIR})
else ()
    message(STATUS "LINUX")
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
endif ()

#add logging module
set(LOGURU_DIR ${TOP_LEVEL_PATH}/3rdparty/loguru)
set(LOGURU_INCLUDE_DIRS ${LOGURU_DIR})
set(LOGURU_SRCS ${LOGURU_DIR}/loguru.cpp)

#Add AFF3CT libraries

# Link with AFF3CT
set (AFF3CT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")
find_package(AFF3CT)



set(LOCAL_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/aff3ct/src
)

############################ 
#       AFF3CT source code
############################ 
set(AFF3CT_ADDONS_SRCS
${CMAKE_CURRENT_SOURCE_DIR}/src/aff3ct-addons/factory/Codec_Generic.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/aff3ct-addons/launcher/Codec.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/aff3ct-addons/launcher/Codec_Generic.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/aff3ct-addons/simulation/Codec.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/aff3ct-addons/simulation/Codec_Generic.cpp
)
    
#set main sources
set(MAIN_SRCS
${CMAKE_CURRENT_SOURCE_DIR}/src/Factory_Codec.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/aff3ct-errc.cpp
)

add_library(aff3ct-codec SHARED ${MAIN_SRCS} ${AFF3CT_ADDONS_SRCS} ${LOGURU_SRCS})
add_executable(aff3ct-server ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp ${MAIN_SRCS} ${AFF3CT_ADDONS_SRCS} ${LOGURU_SRCS})
add_executable(aff3ct-test ${CMAKE_CURRENT_SOURCE_DIR}/src/test.cpp ${LOGURU_SRCS})


target_include_directories(aff3ct-codec PUBLIC ${LOCAL_INCLUDE_DIRS} ${ZeroMQ_INCLUDE_DIR} ${CPPZeroMQ_INCLUDE_DIR} ${FLATBUFFERS_INCLUDE_DIRS} ${FLATBUFFERS_GENERATED_HEADERS_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${LOGURU_INCLUDE_DIRS} aff3ct::aff3ct-static-lib)
#target_include_directories(aff3ct-server PUBLIC ${LOCAL_INCLUDE_DIRS} ${ZeroMQ_INCLUDE_DIR} ${CPPZeroMQ_INCLUDE_DIR} ${FLATBUFFERS_INCLUDE_DIRS} ${FLATBUFFERS_GENERATED_HEADERS_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${LOGURU_INCLUDE_DIRS} AFTER aff3ct::aff3ct-static-lib)
target_include_directories(aff3ct-server PUBLIC ${LOCAL_INCLUDE_DIRS} ${ZeroMQ_INCLUDE_DIR} ${CPPZeroMQ_INCLUDE_DIR} ${FLATBUFFERS_INCLUDE_DIRS} ${FLATBUFFERS_GENERATED_HEADERS_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${LOGURU_INCLUDE_DIRS})
target_include_directories(aff3ct-test PUBLIC ${LOCAL_INCLUDE_DIRS} ${ZeroMQ_INCLUDE_DIR} ${CPPZeroMQ_INCLUDE_DIR} ${FLATBUFFERS_INCLUDE_DIRS} ${FLATBUFFERS_GENERATED_HEADERS_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${LOGURU_INCLUDE_DIRS}  aff3ct::aff3ct-static-lib)

if(NOT WIN32)
    set(DL_LIBRARY dl)
endif()

target_link_libraries(aff3ct-codec PRIVATE aff3ct::aff3ct-static-lib PUBLIC ${ZeroMQ_LIBRARY} ${DL_LIBRARY})
target_link_libraries(aff3ct-server PRIVATE aff3ct::aff3ct-static-lib PUBLIC ${ZeroMQ_LIBRARY} ${DL_LIBRARY})
target_link_libraries(aff3ct-test  PRIVATE aff3ct::aff3ct-static-lib aff3ct-codec PUBLIC ${ZeroMQ_LIBRARY} ${DL_LIBRARY})

add_dependencies(aff3ct-codec aff3ct_proto_generated_includes)
add_dependencies(aff3ct-server aff3ct_proto_generated_includes)
add_dependencies(aff3ct-test aff3ct_proto_generated_includes)

### ADD TESTS
find_package(CxxTest)
#if(CXXTEST_FOUND) 
#    enable_testing()
#    include(unittests/unittests.cmake)
#endif()
