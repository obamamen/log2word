#
#   log2word
#

cmake_minimum_required(VERSION 3.10)
project(log2word VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.cpp")

include_directories(include)

add_executable(log2word ${SOURCES} ${HEADERS})