#pragma once

#include "ixwebsocket/IXWebSocket.h"

#include <string>
#include <zmq.hpp>
#include <nlohmann/json.hpp>

namespace tsrt
{

    // Define a datum
    struct tsrt_dat
    {
        std::string    topic;
        std::string    code;
        // The actual record is not parsed but published to subscriber directly
        nlohmann::json record;
    };

    // Define a message
    struct tsrt_msg
    {
        std::string message;
        bool status;
        tsrt_dat data;
    };

    // Define a request
    struct tsrt_req
    {
        typedef std::map<std::string, std::vector<std::string>> tsrt_req_data_t;
        std::string action;
        std::string token;
        tsrt_req_data_t data;
    };

    class tsrt_pub
    {

        std::string _token;
        std::string _topic;
        std::vector<std::string> _code;

        std::string _url;
        ix::WebSocket _ws;

        std::string _bind;
        zmq::context_t _ctx;
        zmq::socket_t _sock;
        std::mutex _sock_mutex;

        nlohmann::json make_req();

        void pub_raw(const std::string &raw);
        void pub_dat(const tsrt_dat &datum);

    public:
        tsrt_pub(const std::string &token,
                 const std::string &topic = "HQ_STK_TICK",
                 const std::vector<std::string> &code = {"*"},
                 const std::string &url = "wss://ws.waditu.com/listening",
                 const std::string &bind = "tcp://*:6667");

        void run();
    };

} // namespace tsrt