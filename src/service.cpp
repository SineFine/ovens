#include "service.h"

//-------------------------------------------------------
void Service::handleClient() { read_until(); }
//-------------------------------------------------------
void Service::read_until() {
  boost::asio::async_read_until(
      _client->getSocket(), _client->getRecvStreamBuffer(), _client->getDelim(),
      boost::bind(&Service::on_read_until, shared_from_this(),
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));
}
//-------------------------------------------------------
void Service::on_read_until(const boost::system::error_code &ec, size_t) {
  if (ec) {
    ERROR(ec.message());
    return;
  } else {
    write_all();
  }
}
//-------------------------------------------------------
void Service::write_all() {
  _handler.handle();
  boost::asio::async_write(
      _client->getSocket(), boost::asio::buffer(_client->getSendBuffer()),
      boost::asio::transfer_all(),
      boost::bind(&Service::on_write_all, shared_from_this(),
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));
}
//-------------------------------------------------------
void Service::on_write_all(const boost::system::error_code &ec, size_t) {
  if (ec == boost::asio::error::eof) {
  } else if (ec) {
    ERROR(ec.message());
  }
}
//=======================================================
Service::QueryProcessor::QueryProcessor(Service &obj) : _obj(obj) {
  _hndl["gt"s] = boost::bind(&QueryProcessor::gt, this);
  _hndl["sd"s] = boost::bind(&QueryProcessor::sd, this);
  _hndl["gc"s] = boost::bind(&QueryProcessor::gc, this);
}
//-------------------------------------------------------
void Service::QueryProcessor::handle() {
  std::string tmpbuff;
  std::vector<std::string> tmpvector;
  std::istream is(&_obj._client->getRecvStreamBuffer());

  for (std::string tmpbuff; is >> tmpbuff;) {
    _proc_message.push_back(tmpbuff);
  }

  try {
    _hndl[_proc_message[0]]();
  } catch (const std::exception &ec) {
    ERROR(ec.what());
  }
}
//-------------------------------------------------------
void Service::QueryProcessor::gt() {
  _obj._client->getSendBuffer() = timesync::get_duration();
}
//-------------------------------------------------------
void Service::QueryProcessor::sd() {
  std::string date = timesync::to_time_d(_proc_message[2]);
  std::string tmp = _proc_message[1] + " " +
                    std::to_string(std::stoi(_proc_message[2])) + " " + date;
  _obj._stack.push(tmp);
  _obj._client->getSendBuffer() = timesync::get_duration();
}
//-------------------------------------------------------
void Service::QueryProcessor::gc() {
  try {
    std::stringstream sstream;
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini("config.cfg", pt);
    for (const auto &arg : pt.get_child(_proc_message[1])) {
      sstream << arg.first << "=" << arg.second.get_value<std::string>()
              << ", ";
    }
    std::string tmp;
    std::getline(sstream, tmp);
    tmp.erase(tmp.end() - 2, tmp.end());
    tmp.push_back('\t');
    _obj._client->getSendBuffer() = tmp;
  } catch (const boost::property_tree::ptree_bad_path &ec) {
    ERROR(ec.what());
  } catch (const boost::property_tree::ptree_bad_data &ec) {
    ERROR(ec.what());
  } catch (const boost::property_tree::ptree_error &ec) {
    ERROR(ec.what());
  } catch (const std::exception &ec) {
    ERROR(ec.what());
  }
}
