#include "acceptor.h"
//-------------------------------------------------------------------------
Acceptor::Acceptor(boost::asio::io_service &ios, const std::string &ip,
                   unsigned short port)
    : _ios(ios),
      _acceptor(new boost::asio::ip::tcp::acceptor(_ios)),
      _endpoint(new boost::asio::ip::tcp::endpoint),
      _port_def(port),
      _ip_def(ip) {}
//-------------------------------------------------------------------------
bool Acceptor::initAccept() {
  try {
    auto ip_wrong{"0.0.0.0"s};
    auto host = boost::asio::ip::host_name();

    if (_ip_def.compare(ip_wrong) == 0) {
      boost::asio::ip::tcp::resolver resolver(_ios);
      boost::asio::ip::tcp::resolver::query query(host,
                                                  std::to_string(_port_def));
      boost::asio::ip::tcp::resolver::iterator destination =
          resolver.resolve(query);
      boost::asio::ip::tcp::resolver::iterator end;

      for (; destination != end; ++destination) {
        if (!(*destination).endpoint().address().is_v4()) {
          continue;
        } else {
          *_endpoint = *destination;
          break;
        }
      }

      if (!_endpoint->address().to_string().compare(ip_wrong)) {
        throw boost::system::system_error(boost::system::errc::make_error_code(
            boost::system::errc::bad_address));
      }
    } else {
      _endpoint->address(boost::asio::ip::address(
          boost::asio::ip::address::from_string(_ip_def)));
      _endpoint->port(_port_def);
    }
    _acceptor->open(_endpoint->protocol());
    _acceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(false));
    _acceptor->bind(*_endpoint);
    _acceptor->listen(boost::asio::socket_base::max_connections);

  } catch (const boost::system::system_error &ex) {
    ERROR(ex.what());
    stopAccept();
    return _is_accept_init;

  } catch (const std::exception &ex) {
    ERROR(ex.what());
    stopAccept();
    return _is_accept_init;
  }
  accept_init_succ();

  return _is_accept_init;
}
//-------------------------------------------------------------------------
bool Acceptor::startAccept() {
  if (_is_accept_init) {
    try {
      auto client = std::make_shared<Client>(_ios);
      if (!client) {
        ERROR("Unable to allocate memory.");
      } else {
        _acceptor->async_accept(
            client->getSocket(),
            boost::bind(&Acceptor::handle_accept, shared_from_this(),
                        boost::asio::placeholders::error, client));
      }
    } catch (const std::exception &ec) {
      ERROR(ec.what());
      return _is_accept_start;
    }
  } else {
    MESSAGE("Please initialize acceptor first");
    return _is_accept_start;
  }
  change_to_start();
  return _is_accept_start;
}
//-------------------------------------------------------------------------
void Acceptor::stopAccept() {
  boost::system::error_code ec;
  change_to_stop();
  _acceptor->close(ec);
  _is_accept_init = false;
  if (ec) {
    ERROR(ec.message());
  }
}
//-------------------------------------------------------------------------
void Acceptor::handle_accept(const boost::system::error_code &ec,
                             std::shared_ptr<Client> client) {
  if (ec) {
    ERROR(ec.message());
  } else {
    auto service = std::make_shared<Service>(client);

    if (!service) {
      ERROR("Unable to allocate memory");
    } else {
      service->handleClient();
      if (_is_accept_start) {
        startAccept();
      }
    }
  }
}
