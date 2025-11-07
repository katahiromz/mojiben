# libMyLib.a
add_library(MyLib STATIC MyLib.cpp ../read_all.cpp)
target_compile_definitions(MyLib PRIVATE UNICODE _UNICODE)
target_link_libraries(MyLib PRIVATE shlwapi winmm)
