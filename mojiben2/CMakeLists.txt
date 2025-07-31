# CMakeLists.txt --- CMake project settings
##############################################################################

# CMake minimum version
cmake_minimum_required(VERSION 3.10)

# project name and language
project(mojiben2 CXX)

## set output directory (build/)
#set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
#set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

if (WIN32)
    # enable Win32 resource
    enable_language(RC)

    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # using Clang
        set(CMAKE_C_FLAGS "-static")
        set(CMAKE_CXX_FLAGS "-static")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # using GCC
        set(CMAKE_C_FLAGS "-static")
        set(CMAKE_CXX_FLAGS "-static")
    elseif (MSVC)
        # replace "/MD" with "/MT" (building without runtime DLLs)
        set(CompilerFlags
            CMAKE_C_FLAGS
            CMAKE_C_FLAGS_DEBUG
            CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_RELWITHDEBINFO
            CMAKE_CXX_FLAGS
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        foreach(CompilerFlags ${CompilerFlags})
            string(REPLACE "/MD" "/MT" ${CompilerFlags} "${${CompilerFlags}}")
        endforeach()
    endif()
endif()

##############################################################################

if (MSVC)
    add_definitions(-DMSVC)
endif()

# Unicode
add_definitions(-DUNICODE -D_UNICODE)

# mojiben2.exe
add_executable(mojiben2 WIN32 mojiben2.cpp uppercase.cpp lowercase.cpp mojiben2_res.rc)
target_link_libraries(mojiben2 comctl32 winmm shlwapi)
if(MSVC)
    target_link_options(mojiben2 PRIVATE /MANIFEST:NO)
endif()

##############################################################################
