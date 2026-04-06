#pragma once

#include <memory>
#include <string>
#include <tuple>

namespace logger
{
    namespace crypt // 加密器
    {

        std::tuple<std::string, std::string> GenECDHKey(); // 生成ECDH密钥对

        std::string GenECDHSharedSecret(const std::string &client_pri, const std::string &server_pub); // 生成ECDH共享密钥

        std::string BinaryKeyToHex(const std::string &binary_key); // �二进制密钥转换为十六进制字符串

        std::string HexKeyToBinary(const std::string &hex_key); // 十六进制字符串转换为二进制密钥

        class Crypt
        {
        public:
            virtual ~Crypt() = default;  // 析构函数

            virtual void Encrypt(const void *input, size_t input_size, std::string &output) = 0; // 加密数据

            virtual std::string Decrypt(const void *data, size_t size) = 0; // 解密数据
        };

    } // namespace crypt
} // namespace logger
