#include "mytime.h"

namespace timesync {
static std::tm time_start = {0, 0, 0, 1, 0, 100, 0, 0, -1, 0, nullptr};

std::string get_duration() {
  std::time_t timepoin_start = timegm(&time_start);
  std::time_t timepoin_now = std::time(nullptr);
  return std::to_string(timepoin_now - timepoin_start);
}

std::string to_time_n(const std::string &sec) {
  return std::to_string(std::stol(sec) + timegm(&time_start));
}

std::string to_time_d(const std::string &sec) {
  std::stringstream stream;
  std::time_t time_end = std::stol(sec) + timegm(&time_start);
  std::tm time_end_tm = *std::localtime(&time_end);
  stream << std::put_time(&time_end_tm, "%F %T");
  return stream.str();
}
}  // namespace timesync
