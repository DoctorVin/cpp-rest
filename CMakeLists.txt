cmake_minimum_required(VERSION 3.2)
project(cpp-rest-test)

set ( CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} /opt/local )
set(Boost_USE_MULTI_THREADED ON)
find_package( Boost 1.57.0
    REQUIRED unit_test_framework system regex date_time thread filesystem
    program_options chrono atomic )
find_package( OpenSSL )
find_package( Threads )
find_package ( cppnetlib 0.11.0 REQUIRED )
set(CMAKE_VERBOSE_MAKEFILE true)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DBOOST_NETWORK_DEBUG)
endif()

if (OPENSSL_FOUND)
    add_definitions(-DBOOST_NETWORK_ENABLE_HTTPS)
    include_directories(${OPENSSL_INCLUDE_DIR})
endif()

if (${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
elseif (${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
  if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # We want to link in C++11 mode if we're using Clang and on OS X.
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -ftemplate-depth=256 -std=c++11 -stdlib=libc++")
  else()
    # We just add the -Wall and a high enough template depth
    # flag for Clang in other systems.
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -ftemplate-depth=256")
  endif()
endif()

include_directories( /opt/local/include )

enable_testing()

add_subdirectory(server)

#don't forget to run cmake from the build directory! It's easier that way.