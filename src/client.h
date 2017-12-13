#ifndef CLIENT_H
#define CLIENT_H

#include <boost/asio.hpp>
#include <string>

using namespace std::literals;

class Client final {
 public:
  using recv_buffer = std::array<char, 64>;
  using send_buffer = std::string;
  using tcp_socket = boost::asio::ip::tcp::socket;

  explicit Client(boost::asio::io_service& ios)
      : _client(ios), _send_buff("Default Message: 'Hello from server'") {}

  Client(const Client& obj) = delete;
  Client(Client&& obj) = delete;
  Client& operator=(const Client& obj) = delete;
  Client& operator=(Client&& obj) = delete;

 public:
  inline recv_buffer& getRecvBuffer() { return _recv_buffer; }
  inline boost::asio::streambuf& getRecvStreamBuffer() { return _recv_sbuffer; }
  inline send_buffer& getSendBuffer() { return _send_buff; }
  inline tcp_socket& getSocket() { return _client; }
  inline const std::string& getDelim() { return _delim; }

 private:
  tcp_socket _client;
  recv_buffer _recv_buffer;
  send_buffer _send_buff;
  boost::asio::streambuf _recv_sbuffer;
  const std::string _delim {"\r"s};
};

#endif
