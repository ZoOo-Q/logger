# 根目录 CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(logger_first)

# 指定 vcpkg 工具链文件
set(CMAKE_TOOLCHAIN_FILE /home/tudou/respons/vcpkg/scripts/buildsystems/vcpkg.cmake)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加 logger 子目录
add_subdirectory(logger)

# 添加测试子目录
enable_testing()
add_subdirectory(logger/test)