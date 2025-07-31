# CMakeLists.txt --- CMake project settings
##############################################################################

if (MSVC)
    add_definitions(-DMSVC)
endif()

# Unicode
add_definitions(-DUNICODE -D_UNICODE)

# mojiben5.exe
add_executable(mojiben5 WIN32 mojiben5.cpp kanji2.cpp mojiben5_res.rc)
target_link_libraries(mojiben5 comctl32 winmm shlwapi)
if(MSVC)
    target_link_options(mojiben5 PRIVATE /MANIFEST:NO)
endif()

##############################################################################
