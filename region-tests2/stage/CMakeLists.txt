cmake_minimum_required(VERSION 3.16)
project(OpenCVExample LANGUAGES CXX)

# OpenCVを見つける
find_package(OpenCV REQUIRED)

# コンパイル設定
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 実行ファイル
add_executable(OpenCVExample main.cpp)

# OpenCVをリンク
target_link_libraries(OpenCVExample PRIVATE ${OpenCV_LIBS})

# include pathを追加
target_include_directories(OpenCVExample PRIVATE ${OpenCV_INCLUDE_DIRS})
