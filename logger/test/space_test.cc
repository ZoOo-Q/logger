#include <gtest/gtest.h>
#include "../space.h"
namespace logger {
namespace test {

class SpaceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试数据准备
    }

    void TearDown() override {
        // 清理工作
    }
};

// 测试默认构造函数
TEST_F(SpaceTest, DefaultConstructor) {
    bytes b;
    EXPECT_EQ(b.count(), 0);
    kilobytes kb;
    EXPECT_EQ(kb.count(), 0);

    megabytes mb;
    EXPECT_EQ(mb.count(), 0);
}

// 测试带参数的构造函数
TEST_F(SpaceTest, ParameterConstructor) {
    bytes b(1024);
    EXPECT_EQ(b.count(), 1024);

    kilobytes kb(1);
    EXPECT_EQ(kb.count(), 1);

    megabytes mb(2);
    EXPECT_EQ(mb.count(), 2);
}

// 测试运算符重载：++
TEST_F(SpaceTest, IncrementOperator) {
    bytes b(10);
    EXPECT_EQ((++b).count(), 11);
    EXPECT_EQ(b.count(), 11);

    bytes b2(20);
    EXPECT_EQ((b2++).count(), 20);
    EXPECT_EQ(b2.count(), 21);
}

// 测试运算符重载：--
TEST_F(SpaceTest, DecrementOperator) {
    bytes b(10);
    EXPECT_EQ((--b).count(), 9);
    EXPECT_EQ(b.count(), 9);

    bytes b2(20);
    EXPECT_EQ((b2--).count(), 20);
    EXPECT_EQ(b2.count(), 19);
}

// 测试运算符重载：+
TEST_F(SpaceTest, AdditionOperator) {
    bytes b1(100);
    bytes b2(200);
    bytes b3 = b1 + b2;
    EXPECT_EQ(b3.count(), 300);

    kilobytes kb1(1);
    kilobytes kb2(2);
    kilobytes kb3 = kb1 + kb2;
    EXPECT_EQ(kb3.count(), 3);
}

// 测试运算符重载：-
TEST_F(SpaceTest, SubtractionOperator) {
    bytes b1(300);
    bytes b2(100);
    bytes b3 = b1 - b2;
    EXPECT_EQ(b3.count(), 200);

    kilobytes kb1(5);
    kilobytes kb2(2);
    kilobytes kb3 = kb1 - kb2;
    EXPECT_EQ(kb3.count(), 3);
}

// 测试运算符重载：复合赋值
TEST_F(SpaceTest, CompoundAssignmentOperators) {
    bytes b(100);
    b += bytes(200);
    EXPECT_EQ(b.count(), 300);

    b -= bytes(50);
    EXPECT_EQ(b.count(), 250);

    b *= 2;
    EXPECT_EQ(b.count(), 500);

    b /= 5;
    EXPECT_EQ(b.count(), 100);

    b %= 30;
    EXPECT_EQ(b.count(), 10);
}

// 测试单位转换：space_cast
TEST_F(SpaceTest, SpaceCast) {
    // 字节转千字节
    bytes b(1024);
    kilobytes kb = space_cast<kilobytes>(b);
    EXPECT_EQ(kb.count(), 1);

    // 千字节转字节
    kilobytes kb2(2);
    bytes b2 = space_cast<bytes>(kb2);
    EXPECT_EQ(b2.count(), 2048);

    // 千字节转兆字节
    kilobytes kb3(2048);
    megabytes mb = space_cast<megabytes>(kb3);
    EXPECT_EQ(mb.count(), 2);

    // 兆字节转千字节
    megabytes mb2(1);
    kilobytes kb4 = space_cast<kilobytes>(mb2);
    EXPECT_EQ(kb4.count(), 1024);
}

// 测试隐式转换
TEST_F(SpaceTest, ImplicitConversion) {
    // 从 bytes 转换为 kilobytes
    bytes b(2048);
    kilobytes kb = b;
    EXPECT_EQ(kb.count(), 2);

    // 从 kilobytes 转换为 megabytes
    kilobytes kb2(2048);
    megabytes mb = kb2;
    EXPECT_EQ(mb.count(), 2);
}

// 测试不同单位的空间大小比较
TEST_F(SpaceTest, SpaceComparison) {
    // 1 KB = 1024 bytes
    bytes b(1024);
    kilobytes kb(1);
    EXPECT_EQ(space_cast<bytes>(kb).count(), b.count());

    // 1 MB = 1024 KB
    kilobytes kb2(1024);
    megabytes mb(1);
    EXPECT_EQ(space_cast<kilobytes>(mb).count(), kb2.count());
}

// 测试大单位转换
TEST_F(SpaceTest, LargeUnitConversion) {
    // 测试 GB 到 MB
    gigabytes gb(1);
    megabytes mb = space_cast<megabytes>(gb);
    EXPECT_EQ(mb.count(), 1024);

    // 测试 TB 到 GB
    terabytes tb(1);
    gigabytes gb2 = space_cast<gigabytes>(tb);
    EXPECT_EQ(gb2.count(), 1024);
}

} // namespace test
} // namespace logger

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}