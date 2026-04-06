#pragma once

#include <memory>
#include <string>

namespace logger {
namespace compress {

class Compression { // 压缩器
 public:
     virtual ~Compression() = default;

     virtual size_t Compress(const void *input, size_t input_size, void *output, size_t output_size) = 0; // 压缩数据

     virtual size_t CompressedBound(size_t input_size) = 0; // 压缩后的大小上限

     virtual std::string Decompress(const void *data, size_t size) = 0; // 解压缩数据

     virtual void ResetStream() = 0; // 重置压缩器
};

}  // namespace compress
}  // namespace logger
