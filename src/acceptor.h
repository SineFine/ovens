#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <atomic>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/lockfree/stack.hpp>
#include <boost/noncopyable.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "client.h"
#include "macro.h"
#include "service.h"

class Acceptor final : public std::enable_shared_from_this<Acceptor> {
 public:
  Acceptor(boost::asio::io_service &ios, const std::string &ip = "192.168.0.69",
           unsigned short port = 4444);

  Acceptor(const Acceptor &obj) = delete;
  Acceptor(Acceptor &&obj) = delete;
  Acceptor &operator=(const Acceptor &obj) = delete;
  Acceptor &operator=(Acceptor &&obj) = delete;

 public:
  void initAccept();
  void startAccept();
  void stopAccept();

  inline bool getStatus() { return _is_stopped.load(); }

 private:
  void handle_accept(const boost::system::error_code &,
                     std::shared_ptr<Client>);

  inline void accept_init_succ() { _is_accept_init = true; }

  //-------------------------------------------------------
  inline void change_to_stop() { _is_stopped.store(true); }

  inline void change_to_start() { _is_stopped.store(false); }

 private:
  boost::asio::io_service &_ios;
  std::unique_ptr<boost::asio::io_service::work> _work;
  std::unique_ptr<boost::asio::ip::tcp::acceptor> _acceptor;
  std::unique_ptr<boost::asio::ip::tcp::endpoint> _endpoint;
  std::atomic<bool> _is_stopped;
  bool _is_accept_init{false};
  unsigned int _port_def;
  std::string _ip_def;
};

#endif
