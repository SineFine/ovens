#include <mysql/mysql.h>

#include "logos.h"
#include "skeleton.h"

boost::lockfree::stack<std::string> Service::_stack(100);

int main() {
  std::cout << logo_start << std::endl;
  Skeleton Skelet;
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
