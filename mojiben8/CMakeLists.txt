# CMakeLists.txt --- CMake project settings
##############################################################################

if (MSVC)
    add_definitions(-DMSVC)
endif()

# Unicode
add_definitions(-DUNICODE -D_UNICODE)

# mojiben8.exe
add_executable(mojiben8 WIN32 mojiben8.cpp kanji5.cpp readings.cpp mojiben8_res.rc ../Region.cpp)
target_link_libraries(mojiben8 comctl32 winmm shlwapi)
if(MSVC)
    target_link_options(mojiben8 PRIVATE /MANIFEST:NO)
endif()

##############################################################################
