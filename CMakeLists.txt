#
#   log2word
#

cmake_minimum_required(VERSION 3.10)
project(log2word VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.hpp")

include_directories(include)

# Create target only once
add_executable(log2word ${SOURCES} ${HEADERS})

# ---------------------------
# Options for warnings
# ---------------------------
option(ENABLE_WARNINGS "Enable strict compiler warnings" ON)
option(ENABLE_WERROR "Treat warnings as errors" OFF)

if (ENABLE_WARNINGS)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(log2word PRIVATE /W4 /permissive-)
    else()
        set(DEBUG_MIN_FRAME_SIZE 2048) # adjust as needed

        target_compile_options(log2word PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Walloca
                -Wcast-align
                -Wcast-qual
                -Wcomma-subscript
                -Wctor-dtor-privacy
                -Wdeprecated-copy-dtor
                -Wdouble-promotion
                -Wduplicated-branches
                -Wduplicated-cond
                -Wenum-conversion
                -Wextra-semi
                -Wfloat-equal
                -Wformat-overflow=2
                -Wformat-signedness
                -Wformat=2
                -Wframe-larger-than=${DEBUG_MIN_FRAME_SIZE}
                -Wjump-misses-init
                -Wlogical-op
                -Wmismatched-tags
                -Wmissing-braces
                -Wmultichar
                -Wnoexcept
                -Wnon-virtual-dtor
                -Woverloaded-virtual
                -Wpointer-arith
                -Wrange-loop-construct
                -Wrestrict
                -Wshadow
                -Wstrict-null-sentinel
                -Wsuggest-attribute=format
                -Wsuggest-attribute=malloc
                -Wuninitialized
                -Wvla
                -Wvolatile
                -Wwrite-strings
        )

        if (ENABLE_WERROR)
            target_compile_options(log2word PRIVATE -Werror)
        endif()
    endif()
endif()
