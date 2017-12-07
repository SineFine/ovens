#include "acceptor.h"

Acceptor::Acceptor(boost::asio::io_service &ios, const std::string &ip,
                   unsigned short port)
    : _ios(ios),
      _acceptor(new boost::asio::ip::tcp::acceptor(_ios)),
      _endpoint(new boost::asio::ip::tcp::endpoint),
      _is_stopped(true),
      _port_def(port),
      _ip_def(std::move(ip)) {}

void Acceptor::initAccept() {
  try {
    _work.reset(new boost::asio::io_service::work(_ios));
    std::string host = boost::asio::ip::host_name();

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

    if (!_endpoint->address().to_string().compare("0.0.0.0")) {
      throw boost::system::system_error(boost::system::errc::make_error_code(
          boost::system::errc::bad_address));
    }

    _acceptor->open(_endpoint->protocol());
    _acceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(false));
    _acceptor->bind(*_endpoint);
    _acceptor->listen(boost::asio::socket_base::max_connections);

  } catch (const boost::system::system_error &ex) {
    WHAT(ex.what());
    stopAccept();
    return;

  } catch (const std::exception &ex) {
    WHAT(ex.what());
    stopAccept();
    return;
  }

  accept_init_succ();
}

void Acceptor::startAccept() {
  if (_is_accept_init) {
    try {
      auto client = std::make_shared<Client>(_ios);
      if (!client) {
        WHAT("Unable to allocate memory.");
      } else {
        _acceptor->async_accept(
            client->getSocket(),
            boost::bind(&Acceptor::handle_accept, shared_from_this(),
                        boost::asio::placeholders::error, client));
      }
    } catch (const std::exception &ec) {
      WHAT(ec.what());
    }
  } else {
    WHAT("Please initialize acceptor first");
  }
  change_to_start();
}

void Acceptor::stopAccept() {
  boost::system::error_code ec;

  if (!_is_stopped.load()) {
    change_to_stop();
    _acceptor->close(ec);
    _work.reset();
    _ios.stop();
    _is_accept_init = false;
    if (ec) {
      WHAT(ec.message());
    }
  } else {
    WHAT("The acceptor is already stopped. Please run it first.");
  }
}

void Acceptor::handle_accept(const boost::system::error_code &ec,
                             std::shared_ptr<Client> client) {
  if (ec) {
    WHAT(ec.message());
  } else {
    auto service = std::make_shared<Service>(client);

    if (!service) {
      WHAT("Unable to allocate memory");
    } else {
      service->handleClient();
      if (!_is_stopped.load()) {
        startAccept();
      }
    }
  }
}
