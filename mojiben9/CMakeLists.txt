# CMakeLists.txt --- CMake project settings
##############################################################################

if (MSVC)
    add_definitions(-DMSVC)
endif()

# Unicode
add_definitions(-DUNICODE -D_UNICODE)

# mojiben9.exe
add_executable(mojiben9 WIN32 mojiben9.cpp readings.cpp mojiben9_res.rc ../Region.cpp)
target_link_libraries(mojiben9 comctl32 winmm shlwapi MyLib)
if(MSVC)
    target_link_options(mojiben9 PRIVATE /MANIFEST:NO)
endif()
add_dependencies(mojiben9 FuriganaCtl MyLib)

##############################################################################
