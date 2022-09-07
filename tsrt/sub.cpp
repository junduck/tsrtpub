#include "sub.hpp"

namespace tsrt {
using json = nlohmann::json;

void sub::set_action(const std::string &action) { this->action = action; }
void sub::set_token(const std::string &token) { this->token = token; }

void sub::add_subscription(const std::string &topic, const std::string &code) {
  auto it = data.find(topic);
  if (it == data.end()) {
    std::set<std::string> c;
    c.insert(code);
    data.insert(std::make_pair(topic, std::move(c)));
  } else {
    auto &s = it->second;
    s.insert(code);
  }
}

std::string sub::make_subscription() {
  json j;
  j["action"] = action;
  j["token"] = token;
  j["data"] = data;
  return j.dump();
}
} // namespace tsrt
