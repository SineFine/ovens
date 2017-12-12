#include "skeleton.h"

void Skeleton::run() {
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

  _thread_pool.create_thread(std::bind(&Skeleton::write_mysql, this));

  _acceptor->startAccept();
}

void Skeleton::write_mysql() {
  try {
    sql::mysql::MySQL_Driver *driver;
    driver = sql::mysql::get_mysql_driver_instance();

    auto con = std::unique_ptr<sql::Connection>(
        driver->connect("tcp://127.0.0.1:3306", "ovens", "oven"));

    if (con->isValid()) {
      MESSAGE("Conection to the mysql server wass successfull.");
    } else {
      ERROR("Conection to the mysql server filed! ");
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

    while (!_stop) {
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
    ERROR(ec.getSQLStateCStr());
  } catch (const std::exception &ec) {
    ERROR(ec.what());
  }
}

void Skeleton::stop() {
  _acceptor->stopAccept();
  stopReadBuffer();
  _thread_pool.join_all();

  std::cout << "END SERVER" << std::endl;
}
