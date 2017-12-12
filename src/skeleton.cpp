#include "skeleton.h"

//-----------------------------------------------------------------------
Skeleton::Skeleton()
    : _ios(std::make_shared<boost::asio::io_service>()),
      _acceptor(std::make_shared<Acceptor>(*_ios)) {
  _hndl[_hndl_name[0]] = std::bind(&Skeleton::debug, this);
  _hndl[_hndl_name[1]] = std::bind(&Skeleton::job, this);
  _hndl[_hndl_name[2]] = std::bind(&Skeleton::restart, this);
  _hndl[_hndl_name[3]] = std::bind(&Skeleton::quit, this);
}
//-----------------------------------------------------------------------
void Skeleton::runServer() {
  unsigned core = std::thread::hardware_concurrency();
  core = core ? core - 1 : 2u;

  _acceptor->initAccept();

  for (unsigned i = 0; i < core; ++i) {
    _thread_pool.create_thread([this] {
      boost::system::error_code ec;
      _ios->run(ec);
      try {
        if (ec) {
          ERROR(ec.message());
        }
      } catch (const std::exception &ec) {
        ERROR(ec.what());
      }
    });
  }
  _acceptor->startAccept();
}
//-----------------------------------------------------------------------
void Skeleton::stopServer() {
  _acceptor->stopAccept();
  _thread_pool.join_all();
}
//-----------------------------------------------------------------------
void Skeleton::runReadBuffer(const std::string &work) {
  try {
    _hndl[work]();
    MESSAGE("Reading from the buffer started.");
  } catch (const std::exception &ec) {
    ERROR(ec.what());
  }
}
//-----------------------------------------------------------------------

void Skeleton::stopReadBuffer() {
  if (_thread_buff) {
    set_stop_read_buffer();
    _thread_buff->join();
    delete _thread_buff;
    _thread_buff = nullptr;
    MESSAGE("Reading from the buffer is complete.");
  } else {
    WARRNING("Reading from the buffer is not started.");
  }
}

//-----------------------------------------------------------------------
void Skeleton::write_to_mysql() {
  try {
    sql::mysql::MySQL_Driver *driver;
    driver = sql::mysql::get_mysql_driver_instance();

    auto con = std::unique_ptr<sql::Connection>(
        driver->connect("tcp://127.0.0.1:3306", "ovens", "oven"));

    if (con->isValid()) {
      MESSAGE("Conection to the mysql server wass successfully.");
    } else {
      ERROR("Conection to the mysql server filed!");
      return;
    }

    auto &stack = Service::getStack();
    std::string tmpstr;
    std::string id, ymd, hms;
    int second = 0;
    boost::format format("INSERT INTO ovens.%1%(second, date) VALUES (?, ?)");
    auto prep_stmt =
        std::unique_ptr<sql::PreparedStatement>(con->prepareStatement(
            "INSERT INTO ovens.D89C95(second, date) VALUES (?, ?)"));

    while (!_stop_read_buff) {
      if (!stack.empty()) {
        std::stringstream sstream;
        while (stack.pop(tmpstr)) {
          sstream << tmpstr;
          sstream >> id >> second >> ymd >> hms;

          prep_stmt.reset(con->prepareStatement((format % id).str()));
          prep_stmt->setInt(1, second);
          prep_stmt->setString(2, ymd + " " + hms);
          prep_stmt->execute();
        }
      }
      std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    con->close();
    driver->threadEnd();

  } catch (const sql::SQLException &ec) {
    ERROR(ec.what());
  } catch (const std::exception &ec) {
    ERROR(ec.what());
  }
}
//-----------------------------------------------------------------------
void Skeleton::write_to_stdout() {
  auto &stack = Service::getStack();
  std::string tmpstr;
  while (!_stop_read_buff) {
    if (!stack.empty()) {
      while (stack.pop(tmpstr)) {
        std::cout << tmpstr << std::endl;
      }
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }
}
//-----------------------------------------------------------------------
void Skeleton::debug() {
  try {
    if (_thread_buff) {
      set_stop_read_buffer();
      _thread_buff->join();
      set_run_read_buffer();
      delete _thread_buff;
      _thread_buff = new std::thread(std::bind(&Skeleton::write_to_stdout, this));
    } else {
      _thread_buff = new std::thread(std::bind(&Skeleton::write_to_stdout, this));
    }
    _what_is_he_doing = _hndl_name[0];
  } catch (const std::exception &ec) {
    ERROR(ec.what());
  }
}
//-----------------------------------------------------------------------
void Skeleton::job() {
  try {
    if (_thread_buff) {
      set_stop_read_buffer();
      _thread_buff->join();
      set_run_read_buffer();
      delete  _thread_buff;
      _thread_buff = new std::thread (std::bind(&Skeleton::write_to_stdout, this));
    } else {
      _thread_buff = new std::thread (std::bind(&Skeleton::write_to_stdout, this));
    }
    _what_is_he_doing = _hndl_name[1];
  } catch (const std::exception &ec) {
    ERROR(ec.what());
  }
}
//-----------------------------------------------------------------------
void Skeleton::restart() {}
//-----------------------------------------------------------------------
std::istream &operator>>(std::istream &is, Skeleton &obj) {
  std::string istring;
  std::getline(is, istring);
  if (!istring.empty()) {
    try {
      obj._hndl[istring]();
    } catch (const std::exception &ec) {
      ERROR(ec.what());
    }
  }

  return is;
}
//-----------------------------------------------------------------------
std::ostream &operator<<(std::ostream &os, Skeleton &obj) {
  os << "<< [" << obj._what_is_he_doing << "] >> ";
  return os;
}
//-----------------------------------------------------------------------
