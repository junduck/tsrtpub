#include "ixwebsocket/IXNetSystem.h"
#include "tsrt_pub/tsrt_pub.hpp"

int main(int, char**) {
  // required on Windows
  ix::initNetSystem();

  tsrt::tsrt_pub pub("YOUR TUSHARE TOKEN");
  pub.run();

  ix::uninitNetSystem();
  return 0;
}
