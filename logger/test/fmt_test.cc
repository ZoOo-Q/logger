#include <gtest/gtest.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <string>
#include <vector>
#include "fmt/core.h"
#include <string>
#include "../iternal_log.h"
namespace logger {
namespace test {

class FmtTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试数据准备
    }

    void TearDown() override {
        // 清理工作
    }
};

// 测试基本字符串格式化
TEST_F(FmtTest, BasicFormat) {
    std::string result = fmt::format("Hello, {}", "world");
    EXPECT_EQ(result, "Hello, world");
    
    result = fmt::format("{}", 42);
    EXPECT_EQ(result, "42");

    result = fmt::format("{:.2f}", 3.14159);
    EXPECT_EQ(result, "3.14");
}

// 测试多个参数的格式化
TEST_F(FmtTest, MultipleArguments) {
    std::string result = fmt::format("{} + {} = {}", 1, 2, 3);
    EXPECT_EQ(result, "1 + 2 = 3");

    result = fmt::format("Name: {}, Age: {}, Score: {:.1f}", "Alice", 25, 95.5);
    EXPECT_EQ(result, "Name: Alice, Age: 25, Score: 95.5");
}

// 测试命名参数
TEST_F(FmtTest, NamedArguments) {
    // 替换
    std::string result = fmt::format("Hello, {name}!", fmt::arg("name", "Bob"));
    EXPECT_EQ(result, "Hello, Bob!");

    result = fmt::format("{name} is {age} years old", 
                        fmt::arg("name", "Alan"), 
                        fmt::arg("age", 30));
    EXPECT_EQ(result, "Alan is 30 years old");
}

// 测试格式化到字符串
TEST_F(FmtTest, FormatToString) {
    std::string s;
    // 插入
    fmt::format_to(std::back_inserter(s), "Hello, {}", "world");
    EXPECT_EQ(s, "Hello, world");

    s.clear();
    fmt::format_to(std::back_inserter(s), "The answer is {}", 42);
    EXPECT_EQ(s, "The answer is 42");
}



// 测试格式化容器
TEST_F(FmtTest, FormatContainer) {
    std::vector<int> v = {1, 2, 3, 4, 5};
    std::string result = fmt::format("{}", v);
    EXPECT_EQ(result, "[1, 2, 3, 4, 5]");

    std::map<std::string, int> m = {{"ff", 1}, {"bb", 2}};
    result = fmt::format("{}", m);
    EXPECT_TRUE(result == "{\"ff\": 1, \"bb\": 2}" || result == "{\"bb\": 2, \"ff\": 1}");
}



// 测试错误处理
TEST_F(FmtTest, ErrorHandling) {
    // 测试参数数量不匹配
    EXPECT_THROW(fmt::format("Hello, {}!", 1, 2), fmt::format_error);

    // 测试无效的格式说明符
    EXPECT_THROW(fmt::format("{:z}", 42), fmt::format_error);
}

// 测试性能相关的功能
TEST_F(FmtTest, Performance) {
    // 测试重复格式化相同的字符串
    std::string pattern = "Hello, {}!";
    std::string name = "World";
    
    // 执行多次格式化，确保不会崩溃
    for (int i = 0; i < 1000; ++i) {
        std::string result = fmt::format(pattern, name);
        EXPECT_EQ(result, "Hello, World!");
    }
}

} // namespace test
} // namespace logger

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
