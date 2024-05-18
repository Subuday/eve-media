#pragma once

#include <functional>
#include <thread>
#ifdef NO_TLS
#include <websocketpp/config/asio_no_tls_client.hpp>
#else
#include <websocketpp/config/asio_client.hpp>
#endif
#include <websocketpp/client.hpp>
#include <memory>

#include <Semaphore.h>

using namespace std;

namespace ws {
    #ifdef NO_TLS
    using client = websocketpp::client<websocketpp::config::asio_client>;
    #else
    using client = websocketpp::client<websocketpp::config::asio_tls_client>;
    namespace ssl {
        using context = websocketpp::lib::asio::ssl::context;
    }
    #endif
    using connection_hdl = websocketpp::connection_hdl;
    using error_code = websocketpp::lib::error_code;
}

class NetworkClient {
private:
    static const string TAG;  

    ws::client client;
    thread* clientThread;
    Semaphore semaphore;
    ws::connection_hdl chdl;

    function<void(vector<uint8_t>)> onReceiveAudioCallback;

    #ifndef NO_TLS
    shared_ptr<ws::ssl::context> onTslInit(ws::connection_hdl hdl);
    #endif
    void onOpenConnection(ws::connection_hdl hdl);
    void onReceiveMessage(websocketpp::connection_hdl, ws::client::message_ptr msg);
    void onFailConnection(ws::connection_hdl hdl);
    void onCloseConnection(ws::connection_hdl hdl);
public:
    NetworkClient();
    void prepare();
    void setOnReceiveAudioCallback(function<void(vector<uint8_t>)> callback);
    void sendAudio(vector<int8_t> data);
    ~NetworkClient();
};
