# CMakeLists.txt --- CMake project settings
##############################################################################

if (MSVC)
    add_definitions(-DMSVC)
endif()

# Unicode
add_definitions(-DUNICODE -D_UNICODE)

# mojiben6.exe
add_executable(mojiben6 WIN32 mojiben6.cpp mojiben6_res.rc ../Region.cpp)
target_link_libraries(mojiben6 comctl32 winmm shlwapi MyLib)
if(MSVC)
    target_link_options(mojiben6 PRIVATE /MANIFEST:NO)
endif()
add_dependencies(mojiben6 FuriganaCtl MyLib)

##############################################################################
