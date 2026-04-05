# logger/test 目录 CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(logger_tests)

# 指定 vcpkg 工具链文件
set(CMAKE_TOOLCHAIN_FILE /home/tudou/respons/vcpkg/scripts/buildsystems/vcpkg.cmake)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含项目头文件
include_directories(${CMAKE_SOURCE_DIR}/..)

# 查找 googletest
find_package(GTest REQUIRED)
find_package(fmt REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# 添加测试文件
add_executable(mmap_test mmp_test.cc)
add_executable(space_test space_test.cc)
add_executable(fmt_test fmt_test.cc)
add_executable(logmsg_test logmsg_test.cc)



# 链接库
target_link_libraries(mmap_test PRIVATE
    logger
    GTest::gtest
    GTest::gtest_main
    pthread
)

target_link_libraries(space_test PRIVATE
    logger
    GTest::gtest
    GTest::gtest_main
    pthread
)

target_link_libraries(fmt_test PRIVATE
    logger
    GTest::gtest
    GTest::gtest_main
    pthread
    fmt::fmt
)

target_link_libraries(logmsg_test PRIVATE
    logger
    GTest::gtest
    GTest::gtest_main
    pthread
    fmt::fmt
)

# 启用测试
enable_testing()
add_test(NAME mmap_test COMMAND mmap_test)
add_test(NAME space_test COMMAND space_test)
add_test(NAME fmt_test COMMAND fmt_test)
add_test(NAME logmsg_test COMMAND logmsg_test)
