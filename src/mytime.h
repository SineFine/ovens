#ifndef MYTIME_H
#define MYTIME_H

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
//#include <ctime>

namespace timesync {
std::string get_duration();
std::string to_time_n(const std::string &);
std::string to_time_d(const std::string &);
}  // namespace timesync

#endif
