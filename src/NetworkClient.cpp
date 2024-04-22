#include <NetworkClient.h>

NetworkClient::NetworkClient() {
    client.init_asio();
    client.start_perpetual();
    client.set_tls_init_handler([this](websocketpp::connection_hdl hdl) {
        return onTslInit(hdl);    
    });

    t = new thread([this](){
        client.run();
    });

    connect();
}

void NetworkClient::connect() {
    websocketpp::lib::error_code ec;
    ws::client::connection_ptr con = client.get_connection("wss://marketdata.tradermade.com/feedadv", ec);
    if (ec) {
        cout << "Network Client connect initialization error: " << ec.message() << endl;
        return;
    }

    con->set_open_handler(
        [this, c = &client](ws::connection_hdl hdl) {
            this->onOpenConnection(c, hdl);
        }
    );
    con->set_message_handler(
        [this](websocketpp::connection_hdl hdl, auto msg) {
            this->onMessage(hdl, msg);
        }
    );
    con->set_fail_handler(
        [this, c = &client](ws::connection_hdl hdl) {
            this->onOpenConnection(c, hdl);
        }
    );

    client.connect(con);
}

shared_ptr<ws::context> NetworkClient::onTslInit(ws::connection_hdl hdl) {
    shared_ptr<ws::context> ctx = make_shared<ws::context>(ws::context::sslv23);

     try {
        ctx->set_options(
            boost::asio::ssl::context::default_workarounds |
            boost::asio::ssl::context::no_sslv2 |
            boost::asio::ssl::context::no_sslv3 |
            boost::asio::ssl::context::single_dh_use
        );
        ctx->set_verify_mode(boost::asio::ssl::verify_none);
    } catch (std::exception& e) {
        cout << "TLS Initialization Error: " << e.what() << endl;
    }
    return ctx;
}

void NetworkClient::onOpenConnection(ws::client* c, ws::connection_hdl hdl) {
    websocketpp::lib::error_code ec;
    ws::client::connection_ptr con = c->get_con_from_hdl(hdl, ec);
    string payload = "{\"userKey\":\"ws1wY5kyARM8NjrXOtIg\", \"symbol\":\"EURUSD,GBPUSD\"}";
    c->send(con, payload, websocketpp::frame::opcode::text);
}

void NetworkClient::onMessage(websocketpp::connection_hdl, ws::client::message_ptr msg) {
    std::cout << "Received message: " << msg->get_payload() << std::endl;
}

NetworkClient::~NetworkClient() {}
