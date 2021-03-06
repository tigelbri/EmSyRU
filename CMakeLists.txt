###############################################################################
# Declare the minimum cmake version required
###############################################################################
cmake_minimum_required(VERSION 2.8)

###############################################################################
# The name of out project
###############################################################################
project(EmSyRU)

###############################################################################
# Now you would normally declare additional linker and include directories.
# We don't need this at this moment, just to show you how its done
###############################################################################

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)
link_directories(${CMAKE_SOURCE_DIR}/lib)

link_directories("${CMAKE_SOURCE_DIR}/lib")

###############################################################################
# With cmake we can define additional compiler flags for different 
# configurations. CMAKE_CXX_FLAGS are for the default case. The debug config
# can be used to generate debug symbols for gdb. The release option uses
# special optimization flags
###############################################################################

set( CMAKE_CXX_FLAGS         "-std=c++11 -O3 -w " )
set( CMAKE_CXX_FLAGS_RELEASE "-std=c++11 -O3 -msse3 -Wno-deprecated-declarations -Wno-unused -Wcpp" )
set( CMAKE_CXX_FLAGS_DEBUG   "-std=c++11 -g -O0 -Wall" )

###############################################################################
# Find required libraries.Right now we need glut and OpenGL. The required 
# options forces this packages to be present. For non-mandatory packages you 
# can leave this flag out. The can then check if they were found by using
# IF(OpenGL_found) etc.
###############################################################################

find_package(CURL REQUIRED)

if(CURL_FOUND)
    link_directories(${CURL_LIBRARIES_DIRS})
    include_directories(${CURL_INCLUDE_DIR})
endif(CURL_FOUND)

###############################################################################
# Variable for the sources of the binary. In larger projects it is often a 
# good idea to use such variables because it is easy to add more source files
# and you can reuse it for several targets (maybe you want to build a library
# some day ;-)
###############################################################################


set(UPDATER_SOURCES
	src/Updater.cpp
	main.cpp
	src/Logger.cpp
	src/Timestamp.cpp
	src/CurlCommunicator.cpp
	src/UnixBridge.cpp
	src/EmSyRU.cpp
        src/curlpp/Easy.cpp
        src/curlpp/cURLpp.cpp
        src/curlpp/Exception.cpp 
        src/curlpp/Info.cpp
        src/curlpp/Multi.cpp
        src/curlpp/OptionBase.cpp
        src/curlpp/Options.cpp
        src/curlpp/Form.cpp
        src/curlpp/internal/CurlHandle.cpp
        src/curlpp/internal/OptionList.cpp
        src/curlpp/internal/OptionSetter.cpp
        src/curlpp/internal/SList.cpp
)

###############################################################################
# The executable fpr our project
###############################################################################

add_executable(EmSyRU ${UPDATER_SOURCES})

###############################################################################
# External library dependencys
###############################################################################

target_link_libraries(EmSyRU curl)


