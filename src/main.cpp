#include <mysql/mysql.h>

#include "skeleton.h"
#include "logos.h"

boost::lockfree::stack<std::string> Service::_stack(100);

int main() {
  std::cout << logo_start << std::endl;
  Skeleton skeleton;
  skeleton.run();
  std::cin.get();
  skeleton.stop();
  std::cout << logo_end << std::endl;

  return 0;
}
