#pragma once

#include "nlohmann/json.hpp"
#include <map>
#include <set>
#include <string>

namespace tsrt {

class sub {
  std::string action;
  std::string token;
  std::map<std::string, std::set<std::string>> data;

public:
  void set_action(const std::string &action);
  void set_token(const std::string &token);

  template <typename CONTAINER>
  void add_subscription(const std::string &topic, const CONTAINER &codes) {
    auto it = data.find(topic);
    if (it == data.end()) {
      data[topic] = std::set<std::string>(codes.begin(), codes.end());
    } else {
      for (auto const &code : codes) {
        it->second.insert(code);
      }
    }
  }
  void add_subscription(const std::string &topic, const std::string &code);

  std::string make_subscription();
};

} // namespace tsrt
