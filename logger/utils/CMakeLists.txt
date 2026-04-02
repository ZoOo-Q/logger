# utils 目录 CMakeLists.txt

cmake_minimum_required(VERSION 3.10)

# 编译 utils 相关文件
set(UTILS_SOURCES
    file_util.cc
)

# 将这些文件添加到 logger 库
set(UTILS_HEADERS
    file_util.h
)

# 安装头文件
install(FILES ${UTILS_HEADERS} DESTINATION include/logger/utils)