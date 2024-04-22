#include <thread>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <memory>

namespace ws {
    using client = websocketpp::client<websocketpp::config::asio_tls_client>;
    using context = websocketpp::lib::asio::ssl::context;
    typedef websocketpp::connection_hdl connection_hdl;
}

using namespace std;

class NetworkClient {
private:
    thread* t;
    ws::client client;
   
    void connect();
    shared_ptr<ws::context> onTslInit(ws::connection_hdl hdl);
    void onOpenConnection(ws::client* c, ws::connection_hdl hdl);
    void onMessage(websocketpp::connection_hdl, ws::client::message_ptr msg);
 // void onCloseConnection(ws::client* c, ws::connection_hdl hdl);
public:
    NetworkClient();
    ~NetworkClient();
// private:
    // void on_message(websocketpp::connection_hdl, client::message_ptr msg);
    // void init();

    // std::string uri_;
    // websocketpp::connection_hdl connection_handle_;
    // std::thread client_thread_;
    // bool initialized_;
};
