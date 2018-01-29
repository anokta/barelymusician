#include "logging.h"

namespace barelyapi {

Logger::Logger(std::ostream& out, const char* file, int line) : out_(out) {
  out_ << "[" << file << ":" << line << "] ";
}

Logger::~Logger() { out_ << std::endl; }

std::ostream& Logger::GetStream() { return out_; }

std::ostream& Logger::NullStream() {
  static std::ostream null_out(nullptr);
  return null_out;
}

}  // namespace barelyapi
