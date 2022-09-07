#include "ixwebsocket/IXNetSystem.h"
#include "ixwebsocket/IXWebSocket.h"
#include "tsrt/sub.hpp"
#include <fstream>
#include <iostream>
#include <zmq.hpp>

int main(int, char **) {
  // required on Windows
  ix::initNetSystem();

  std::ifstream ifs("config.json");
  auto config = nlohmann::json::parse(ifs);

  tsrt::sub s;
  s.set_action("listening");
  s.set_token(config["token"]);
  for (auto const &sub : config["sub"].items()) {
    s.add_subscription(sub.key(), sub.value());
  }
  std::cerr << s.make_subscription() << std::endl;

  zmq::context_t ctx(1);
  zmq::socket_t sock(ctx, ZMQ_PUB);
  sock.set(zmq::sockopt::sndhwm, 10000);
  sock.bind(config["pub"]["bind"]);

  ix::WebSocket ws;
  ws.setUrl(config["url"]);
  ws.setPingInterval(30);
  ws.setMaxWaitBetweenReconnectionRetries(1);
  ws.setOnMessageCallback([&](const ix::WebSocketMessagePtr &msg) {
    if (msg->type == ix::WebSocketMessageType::Message) {
      auto j = nlohmann::json::parse(msg->str);
      if (j["status"]) {
        auto data = j["data"];
        std::string code = data["code"].dump();
        std::string record = data["record"].dump();
        zmq::message_t env(code.data(), code.size());
        zmq::message_t rec(record.data(), record.size());
        sock.send(env, zmq::send_flags::sndmore);
        sock.send(rec, zmq::send_flags::none);
        std::cout << "DATA: " << code << " " << record << std::endl;
      } else {
        std::cerr << "Error: " << j["message"].dump() << std::endl;
      }
    } else if (msg->type == ix::WebSocketMessageType::Open) {
      ws.sendText(s.make_subscription());
    } else if (msg->type == ix::WebSocketMessageType::Error) {
      std::cerr << "Error: [" << msg->errorInfo.http_status << "] "
                << msg->errorInfo.reason << std::endl;
    }
  });

  // just block
  ws.run();

  ix::uninitNetSystem();
  return 0;
}
