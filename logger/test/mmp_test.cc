#include <gtest/gtest.h>
#include "../mmap/mmap_aux.h"
#include "../utils/file_util.h"
#include <filesystem>
#include <string>
#include <cstring>

namespace logger {
namespace mmap {
namespace test {

class MMapAuxTest : public ::testing::Test {
protected:
    // 测试文件路径
    std::filesystem::path test_file_;

    void SetUp() override {
        // 创建临时测试文件
        test_file_ = std::filesystem::temp_directory_path() / "test_mmap_aux.log";
        // 确保文件不存在
        if (std::filesystem::exists(test_file_)) {
            std::filesystem::remove(test_file_);
        }
    }

    void TearDown() override {
        // 清理临时文件
        if (std::filesystem::exists(test_file_)) {
            std::filesystem::remove(test_file_);
        }
    }
};

// 测试创建和初始化
TEST_F(MMapAuxTest, CreateAndInit) {
    // 创建 MMapAux 实例
    MMapAux mmap_aux(test_file_.string());
    
    // 验证内存映射是否有效
    EXPECT_TRUE(mmap_aux.IsValid_());
    
    // 验证初始大小
    EXPECT_EQ(mmap_aux.Size(), 0);
}

// 测试数据写入和读取
TEST_F(MMapAuxTest, PushAndRead) {
    MMapAux mmap_aux(test_file_.string());
    
    // 写入测试数据
    const char* test_data = "Hello, MMap!";
    size_t data_size = strlen(test_data);
    mmap_aux.Push(test_data, data_size);
    
    // 验证大小更新
    EXPECT_EQ(mmap_aux.Size(), data_size);
    
    // 读取数据并验证
    char buffer[100] = {0};
    memcpy(buffer, mmap_aux.Data(), mmap_aux.Size());
    EXPECT_STREQ(buffer, test_data);
}

// 测试扩容功能
TEST_F(MMapAuxTest, Resize) {
    MMapAux mmap_aux(test_file_.string());
    
    // 写入大量数据，触发扩容
    std::string large_data(1024 * 10, 'a'); // 10KB 数据
    mmap_aux.Push(large_data.data(), large_data.size());
    
    // 验证大小
    EXPECT_EQ(mmap_aux.Size(), large_data.size());
    
    // 读取数据并验证
    char buffer[1024 * 10 + 1] = {0};
    memcpy(buffer, mmap_aux.Data(), mmap_aux.Size());
    EXPECT_STREQ(buffer, large_data.c_str());
}

// 测试同步到磁盘
TEST_F(MMapAuxTest, Sync) {
    MMapAux mmap_aux(test_file_.string());
    
    // 写入测试数据
    const char* test_data = "Test sync to disk";
    size_t data_size = strlen(test_data);
    mmap_aux.Push(test_data, data_size);
    
    // 同步到磁盘
    mmap_aux.Sync_();
    
    // 重新创建 MMapAux 实例，验证数据持久化
    MMapAux new_mmap_aux(test_file_.string());
    EXPECT_TRUE(new_mmap_aux.IsValid_());
    EXPECT_EQ(new_mmap_aux.Size(), data_size);
    
    // 读取数据并验证
    char buffer[100] = {0};
    memcpy(buffer, new_mmap_aux.Data(), new_mmap_aux.Size());
    EXPECT_STREQ(buffer, test_data);
}

// 测试无效状态处理
TEST_F(MMapAuxTest, InvalidState) {
    // 默认构造的 MMapAux 应该无效
    MMapAux mmap_aux;
    EXPECT_FALSE(mmap_aux.IsValid_());
    
    // 尝试在无效状态下操作，应该不会崩溃
    const char* test_data = "Test";
    mmap_aux.Push(test_data, strlen(test_data));
    mmap_aux.Resize(100);
    mmap_aux.Sync_();
}

// 测试容量管理
TEST_F(MMapAuxTest, Capacity) {
    MMapAux mmap_aux(test_file_.string());
    
    // 验证初始容量
    size_t initial_capacity = mmap_aux.Capacity_();
    EXPECT_GT(initial_capacity, 0);
    
    // 写入数据后容量应该不变
    const char* test_data = "Test capacity";
    mmap_aux.Push(test_data, strlen(test_data));
    EXPECT_EQ(mmap_aux.Capacity_(), initial_capacity);
}

}  // namespace test
}  // namespace mmap
}  // namespace logger

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}