#include <gtest/gtest.h>
#include "../log_msg.h"



namespace logger {
namespace test {

class LogMsgTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试数据准备
    }

    void TearDown() override {
        // 清理工作
    }
};

// 测试基本字符串格式化
TEST_F(LogMsgTest, BasicFormat) { 
    SourceLocation loc{"test_file.cpp", 10, "test_function"};      
    LogLevel level = LogLevel::kInfo;
    StringView msg = "This is a log message.";
    LogMsg log_msg(loc, level, msg);
    EXPECT_EQ(log_msg.location.file_name, "test_file.cpp");
    EXPECT_EQ(log_msg.location.line, 10);   
    EXPECT_EQ(log_msg.location.func_name, "test_function");
    EXPECT_EQ(log_msg.level, LogLevel::kInfo);
    EXPECT_EQ(log_msg.message, "This is a log message.");
}

// 测试多个参数的格式化


} // namespace test
} // namespace logger

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
