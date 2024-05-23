#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <queue>
#include <iostream>
#include <vector>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
using namespace std;

class Session: public enable_shared_from_this<Session> {
private:
    static const string TAG;

    tcp::resolver resolver;
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws;
    net::strand<net::io_context::executor_type> strand;
    beast::flat_buffer buffer;
    std::string host;
    queue<vector<int8_t>> q;

    void onResolve(beast::error_code ec, tcp::resolver::results_type results);
    void onConnect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep);
    void onSslHandshake(beast::error_code ec);
    void onHandshake(beast::error_code ec);
    void onRead(beast::error_code ec, size_t bytes_transferred);
    void onWrite(beast::error_code ec, std::size_t);
public:
    Session(net::io_context& ioc, ssl::context& ctx);
    ~Session();

    void run(char const* host, char const* port);
    void send(vector<int8_t>& data);
};
