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

#include "acceptor.h"

class Skeleton {
 public:
  Skeleton()
      : _ios(std::make_shared<boost::asio::io_service>()),
        _acceptor(std::make_shared<Acceptor>(*_ios)) {}

 public:
  void run();
  void stop();

 private:
  void write_mysql();

  inline void stopReadBuffer() { _stop = true; }

 private:
  std::shared_ptr<boost::asio::io_service> _ios;
  std::shared_ptr<Acceptor> _acceptor;
  boost::thread_group _thread_pool;
  std::atomic_bool _stop{false};
};

#endif
