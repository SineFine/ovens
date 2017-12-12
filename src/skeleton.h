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

class Skeleton {
  using handler = std::unordered_map<std::string, std::function<void()>>;

 public:
  Skeleton();

  Skeleton(const Skeleton &) = delete;
  Skeleton &operator=(const Skeleton &) = delete;
  Skeleton(Skeleton &&) = delete;
  Skeleton &operator=(Skeleton &&) = delete;

  friend std::istream &operator>>(std::istream &, Skeleton &);
  friend std::ostream &operator<<(std::ostream &, Skeleton &);
  inline operator bool() { return _is_stop; }

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
  void quit() { _is_stop = true; }
  void debug();
  void job();
  void restart();

 private:
  std::shared_ptr<boost::asio::io_service> _ios;
  std::shared_ptr<Acceptor> _acceptor;
  boost::thread_group _thread_pool;
  std::thread *_thread_buff{nullptr};
  handler _hndl;

  std::vector<std::string> _hndl_name{"debug"s, "job"s, "restart"s, "quit"s};
  std::atomic_bool _stop_read_buff{false};
  bool _is_stop{false};
  std::string _what_is_he_doing{"none"s};
};

#endif
