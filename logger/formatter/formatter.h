#include "../log_common.h"
#include "../log_msg.h"

namespace logger {

class Formatter { // 日志格式化器
 public:
  virtual ~Formatter() = default;

  virtual void Format(const LogMsg& msg, MemoryBuf* dest) = 0;
};

}  // namespace logger

