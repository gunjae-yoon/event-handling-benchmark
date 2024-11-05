cmake_minimum_required(VERSION 3.20)
project(event-handling-benchmark CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include (cmake/version.cmake)

message("= CMAKE STATUS =============================================")
message("    Version      : ${PROJECT_VERSION_DETAIL}")
message("    Compiler     : ${CMAKE_CXX_COMPILER}")
message("    Build type   : ${CMAKE_BUILD_TYPE}")
message("    Source path  : ${CMAKE_SOURCE_DIR}")
message("    Install path : ${CMAKE_INSTALL_PREFIX}")
message("============================================================")

include (cmake/event-handling-benchmark.cmake)
