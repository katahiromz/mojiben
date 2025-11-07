# CMakeLists.txt --- CMake project settings
##############################################################################

if (MSVC)
    add_definitions(-DMSVC)
endif()

# Unicode
add_definitions(-DUNICODE -D_UNICODE)

# mojiben7.exe
add_executable(mojiben7 WIN32 mojiben7.cpp mojiben7_res.rc ../Region.cpp)
target_link_libraries(mojiben7 comctl32 winmm shlwapi MyLib)
if(MSVC)
    target_link_options(mojiben7 PRIVATE /MANIFEST:NO)
endif()
add_dependencies(mojiben7 FuriganaCtl MyLib)

##############################################################################
