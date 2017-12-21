#ifndef SKELETON_H
#define SKELETON_H

#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/resultset_metadata.h>
#include <cppconn/statement.h>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <mysql_error.h>
#include <atomic>
#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread_pool.hpp>
#include <exception>
#include <functional>
#include <memory>
#include <sstream>
#include <thread>
#include <unordered_map>

#include "acceptor.h"
#include "macro.h"

class Skeleton;

class control_properties
{
  std::string _ip{};
  unsigned short _port{};
  std::string _mysql_loggin{};
  std::string _mysql_passwd{};

  friend Skeleton;

public:
  control_properties& ip(const std::string &ip){
    _ip = std::move(ip); return *this;
  }

  control_properties& port(const unsigned short &port){
    _port = port; return *this;
  }

  control_properties& mysql_login(const std::string login){
    _mysql_loggin = std::move(login); return *this;
  }

  control_properties& mysql_passwd(const std::string passwd){
    _mysql_passwd = std::move(passwd); return *this;
  }

};

class Skeleton {
  using handler = std::unordered_map<std::string, std::function<void()>>;

 public:
  Skeleton();
  Skeleton(const control_properties &cp);

  Skeleton(const Skeleton &) = delete;
  Skeleton &operator=(const Skeleton &) = delete;
  Skeleton(Skeleton &&) = delete;
  Skeleton &operator=(Skeleton &&) = delete;

  friend std::istream &operator>>(std::istream &, Skeleton &);
  friend std::ostream &operator<<(std::ostream &, Skeleton &);
  explicit operator bool() { return _is_stop; }

 public:
  void runServer();
  void stopServer();

  void runReadBuffer(const std::string & = "debug"s);
  void stopReadBuffer();

 private:
  void write_to_mysql();
  void write_to_stdout();

  inline void set_stop_read_buffer() { _stop_read_buff = true; }
  inline void set_run_read_buffer() { _stop_read_buff = false; }

 private:
  void create_io_threade();

 private:
  void quit() { _is_stop = true; }
  void debug();
  void job();
  void restart();

 private:
  std::string _ip{};
  unsigned short _port{};
  std::string _mysql_loggin{};
  std::string _mysql_passwd{};

  std::unique_ptr<boost::asio::io_service> _ios;
  std::unique_ptr<boost::asio::io_service::work> _work;
  std::shared_ptr<Acceptor> _acceptor;
  std::unique_ptr<boost::thread_group> _thread_pool;

  std::thread *_thread_buff{nullptr};
  handler _hndl;

  std::vector<std::string> _hndl_name{"debug"s, "job"s, "restart"s, "quit"s};
  std::atomic_bool _stop_read_buff{false};
  bool _is_stop{false};
  std::string _what_is_he_doing{"none"s};

};

#endif
