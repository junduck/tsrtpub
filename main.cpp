#include "ixwebsocket/IXNetSystem.h"
#include "tsrt_pub/tsrt_pub.hpp"

int main(int, char **)
{
  // required on Windows
  ix::initNetSystem();

  tsrt::tsrt_pub pub("YOUR TUSHARE TOKEN",
                     "YOUR REALTIME TOPIC, e.g. HQ_STK_TICK",
                     {"YOUR SUBSCRIBED CODES, AS A VECTOR OF STRING, e.g. {*}"},
                     "wss://ws.waditu.com/listening",
                     "YOUR PUBLISHER BINDING ADDRESS, e.g. tcp://*:6667");
  pub.run();

  ix::uninitNetSystem();
  return 0;
}
