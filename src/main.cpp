#include <mysql/mysql.h>
#include <string>

#include "logos.h"
#include "skeleton.h"

using namespace std::string_literals;

boost::lockfree::stack<std::string> Service::_stack(100);

int main() {
  std::cout << logo_start << std::endl;
  Skeleton Skelet(control_properties()
                      .ip("0.0.0.0"s)
                      .port(2544)
                      .mysql_login("ovens"s)
                      .mysql_passwd("oven"s));
  Skelet.runServer();
  Skelet.runReadBuffer();

  do {
    std::cout << Skelet;
    std::cin >> Skelet;
  } while (!Skelet);

  Skelet.stopServer();
  Skelet.stopReadBuffer();
  std::cout << logo_end << std::endl;
  return 0;
}
