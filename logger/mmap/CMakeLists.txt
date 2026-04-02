# logger/mmap 目录 CMakeLists.txt

cmake_minimum_required(VERSION 3.10)

# 编译 mmap 相关文件
set(MMAP_SOURCES
    mmap_aux.cc
)

# 头文件
set(MMAP_HEADERS
    mmap_aux.h
)

# 安装头文件
install(FILES ${MMAP_HEADERS} DESTINATION include/logger/mmap)