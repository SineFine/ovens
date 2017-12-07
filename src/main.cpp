#include <mysql/mysql.h>

#include "skeleton.h"

boost::lockfree::stack<std::string> Service::_stack(100);

int main() {
  Skeleton skeleton;
  skeleton.run();
  std::cin.get();
  skeleton.stop();

  return 0;
}
