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
#include "macro.h"
#include "mytime.h"

using namespace std::literals;

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
  void read_until();
  void on_read_until(const boost::system::error_code &, size_t);
  void write_all();
  void on_write_all(const boost::system::error_code &, size_t);

 private:
  std::shared_ptr<Client> _client;
  QueryProcessor _handler;

 private:
  static boost::lockfree::stack<std::string> _stack;
};

#endif
