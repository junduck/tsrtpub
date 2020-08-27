#include "tsrt_pub.hpp"

#include <chrono>
#include <ctime>

#include <iostream>

namespace tsrt
{

    using nlohmann::json;

    // Parsing tsrt_dat
    void to_json(json &j, const tsrt_dat &dat)
    {
        j = json{
            {"topic", dat.topic},
            {"code", dat.code},
            {"record", dat.record}};
    }
    void from_json(const json &j, tsrt_dat &dat)
    {
        j.at("topic").get_to(dat.topic);
        j.at("code").get_to(dat.code);
        j.at("record").get_to(dat.record);
    }

    // Parsing tsrt_msg
    void to_json(json &j, const tsrt_msg &msg)
    {
        j = json{
            {"message", msg.message},
            {"status", msg.status},
            {"data", msg.data}};
    }
    void from_json(const json &j, tsrt_msg &msg)
    {
        j.at("message").get_to(msg.message);
        j.at("status").get_to(msg.status);
        j.at("data").get_to(msg.data);
    }

    // Parsing tsrt_req
    void to_json(json &j, const tsrt_req &req)
    {
        j = json{
            {"action", req.action},
            {"token", req.token},
            {"data", req.data}};
    }
    void from_json(const json &j, tsrt_req &req)
    {
        j.at("action").get_to(req.action);
        j.at("token").get_to(req.token);
        j.at("data").get_to(req.data);
    }

    json tsrt_pub::make_req()
    {
        tsrt_req::tsrt_req_data_t req_data{
            {_topic, _code}};
        json j{
            {"action", "listening"},
            {"token", _token},
            {"data", req_data}};
        return j;
    }

    void tsrt_pub::pub_raw(const std::string &raw)
    {
        if (raw.size())
        {
            std::lock_guard<std::mutex> lock(_sock_mutex);
            zmq::message_t msg(raw.data(), raw.size());
            _sock.send(msg, zmq::send_flags::none);
        }
    }

    void tsrt_pub::pub_dat(const tsrt_dat &datum)
    {
        auto str = datum.record.dump();
        if (str.size())
        {
            std::lock_guard<std::mutex> lock(_sock_mutex);
            // send envelope
            zmq::message_t env(datum.code.data(), datum.code.size());
            _sock.send(env, zmq::send_flags::sndmore);
            // send record
            zmq::message_t rec(str.data(), str.size());
            _sock.send(rec, zmq::send_flags::none);
        }
    }

    void tsrt_pub::run()
    {
        _ws.setOnMessageCallback([this](const ix::WebSocketMessagePtr &msg) {
            auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            if (msg->type == ix::WebSocketMessageType::Message)
            {
                json j = json::parse(msg->str);
                if (j["status"])
                {
                    tsrt_dat datum = j["data"];
                    pub_dat(datum);
                }
                else
                {
                    std::cout << std::ctime(&t) << "Error: " << j["message"] << std::endl;
                }
            }
            else if (msg->type == ix::WebSocketMessageType::Ping ||
                     msg->type == ix::WebSocketMessageType::Pong)
            {
                std::cout << std::ctime(&t) << " PING" << std::endl;
            }
            else if (msg->type == ix::WebSocketMessageType::Open)
            {
                json j = make_req();
                _ws.sendText(j.dump());
                std::cout << std::ctime(&t) << " OPEN" << std::endl;
            }
            else if (msg->type == ix::WebSocketMessageType::Close)
            {
                std::cout << std::ctime(&t) << " CLOSE" << std::endl;
            }
            else
            {
                std::cout << std::ctime(&t) << " Error: " << msg->errorInfo.http_status << " " << msg->errorInfo.reason << std::endl;
            }
        });
        _ws.run();
    }

    tsrt_pub::tsrt_pub(const std::string &token,
                       const std::string &topic,
                       const std::vector<std::string> &code,
                       const std::string &url,
                       const std::string &bind)
        : _token(token),
          _topic(topic),
          _code(code),
          _url(url),
          _bind(bind),
          _ctx(1),
          _sock(_ctx, zmq::socket_type::pub)
    {
        _ws.setUrl(_url);
        _ws.setPingInterval(15);
        _ws.disablePerMessageDeflate();
        _ws.setMaxWaitBetweenReconnectionRetries(1);
        _sock.bind(_bind);
    }
} // namespace tsrt