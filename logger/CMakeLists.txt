# logger/CMakeLists.txt

cmake_minimum_required(VERSION 3.10)

# 创建 logger 库
add_library(logger
    mmap/mmap_aux.cc
    mmap/mmap_linux.cc
    utils/file_util.cc
)

# 包含头文件目录
target_include_directories(logger PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/mmap
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

# 链接系统库
target_link_libraries(logger PRIVATE
    pthread
)