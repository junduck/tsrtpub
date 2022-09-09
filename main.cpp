#include "ixwebsocket/IXNetSystem.h"
#include "ixwebsocket/IXWebSocket.h"
#include "tsrt/sub.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
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
  bool ws_running{false}; // we dont need atomic or lock here

  ws.setUrl(config["url"]);
  ws.setPingInterval(30);
  ws.setMaxWaitBetweenReconnectionRetries(1000);
  ws.setOnMessageCallback([&](const ix::WebSocketMessagePtr &msg) {
    if (ws_running) {
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
        } else {
          std::cerr << "Error: " << j["message"].dump() << std::endl;
        }
      } else if (msg->type == ix::WebSocketMessageType::Open) {
        ws.sendText(s.make_subscription());
      } else if (msg->type == ix::WebSocketMessageType::Error) {
        std::cerr << "Error: [" << msg->errorInfo.http_status << "] "
                  << msg->errorInfo.reason << std::endl;
        ws_running = false;
      }
    }
  });

  ws_running = true;
  ws.start();

  std::thread t2([&]() {
    while (true) {
      {
        if (!ws_running) {
          ws.stop();
          break;
        }
      }
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  });

  if (t2.joinable()) {
    t2.join();
  }

  ix::uninitNetSystem();
  return 0;
}
