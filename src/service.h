#ifndef SERVICE_H
#define SERVICE_H

#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/lockfree/stack.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/thread.hpp>
#include <chrono>
#include <exception>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

#include "client.h"

#define WHAT(x)                                                           \
  std::cerr << "[[ Error occure in: "                                     \
            << "function( " << __FUNCTION__ << " ) \\ line( " << __LINE__ \
            << " ) ]]\n"                                                  \
            << "[[ Message: " << (x) << " ]]" << std::endl;

#define WHEREIS()                                                         \
  std::cout << "{{ A am in: "                                             \
            << "function( " << __FUNCTION__ << " ) \\ line( " << __LINE__ \
            << " ) }}\n"

using namespace std::literals;

//--------------------------------------------------------------------
namespace timesync {
static std::tm time_start = {0, 0, 0, 1, 0, 100, 0, 0, -1, 0, nullptr};

inline std::string get_duration() {
  std::time_t timepoin_start = timegm(&time_start);
  std::time_t timepoin_now = std::time(nullptr);
  return std::to_string(timepoin_now - timepoin_start);
}

inline std::string to_time_n(std::string sec) {
  return std::to_string(std::stoi(sec) + timegm(&time_start));
}

inline std::string to_time_d(std::string sec) {
  std::stringstream stream;
  std::time_t time_end = std::stol(sec) + timegm(&time_start);
  std::tm time_end_tm = *gmtime(&time_end);
  stream << std::put_time(&time_end_tm, "%F %T");
  return stream.str();
}
}  // namespace timesync

//--------------------------------------------------------------
class Service : public std::enable_shared_from_this<Service> {
 private:
  class QueryProcessor {
    using handlers = std::unordered_map<std::string, std::function<void()>>;

   public:
    explicit QueryProcessor(Service &obj);

    QueryProcessor(const QueryProcessor &obj) = delete;
    QueryProcessor(QueryProcessor &&obj) = delete;
    QueryProcessor &operator=(const QueryProcessor &obj) = delete;
    QueryProcessor &operator=(QueryProcessor &&obj) = delete;

   public:
    void handle();

   private:
    void gt();
    void sd();
    void gc();

   private:
    Service &_obj;
    handlers _hndl;
    std::vector<std::string> _proc_message;
  };

 public:
  explicit Service(std::shared_ptr<Client> client)
      : _client(client), _handler(*this) {}

  Service(const Service &obj) = delete;
  Service(Service &&obj) = delete;
  Service &operator=(const Service &obj) = delete;
  Service &operator=(Service &&obj) = delete;

 public:
  void handleClient();

  inline static boost::lockfree::stack<std::string> &getStack() {
    return Service::_stack;
  }

 private:
  void read_all();
  void write_all();
  void read_write_all();
  void on_read_all(const boost::system::error_code &, size_t);
  void on_write_all(const boost::system::error_code &, size_t);
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  void read_until();
  void read_write_until();
  void on_read_until(const boost::system::error_code &, size_t);

 private:
  std::shared_ptr<Client> _client;
  QueryProcessor _handler;

 private:
  static boost::lockfree::stack<std::string> _stack;
};
//---------------------------------------------------------------

#endif
