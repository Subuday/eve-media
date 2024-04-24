#include <NetworkClient.h>

const string NetworkClient::TAG = "[NetworkClient] ";

NetworkClient::NetworkClient() : semaphore(0) {}

void NetworkClient::prepare() {
    client.clear_access_channels(websocketpp::log::alevel::frame_header);
    client.clear_access_channels(websocketpp::log::alevel::frame_payload);

    client.init_asio();
    client.start_perpetual();
    #ifndef NO_TLS
    client.set_tls_init_handler([this](ws::connection_hdl hdl) {
        return onTslInit(hdl);    
    });
    #endif

    clientThread = new thread([this](){
        client.run();
    });

    ws::error_code ec;
    ws::client::connection_ptr con = client.get_connection("ws://localhost:8765", ec);
    if (ec) {
        cout << TAG << "Connection initialization error: " << ec.message() << endl;
        return;
    }
    cout << TAG << "Connection initialization successful" << endl;

    con->set_open_handler(
        [this](ws::connection_hdl hdl) {
            this->onOpenConnection(hdl);
        }
    );
    con->set_message_handler(
        [this](websocketpp::connection_hdl hdl, auto msg) {
            this->onReceiveMessage(hdl, msg);
        }
    );
    con->set_fail_handler(
        [this](ws::connection_hdl hdl) {
            this->onFailConnection(hdl);
        }
    );
    con->set_close_handler(
        [this](ws::connection_hdl hdl) {
            this->onCloseConnection(hdl);
        }
    );

    client.connect(con);

    semaphore.acquire();
}

void NetworkClient::setOnReceiveAudioCallback(function<void(vector<uint8_t>)> callback) {
    onReceiveAudioCallback = callback;
}

#ifndef NO_TLS
shared_ptr<ws::ssl::context> NetworkClient::onTslInit(ws::connection_hdl hdl) {
    shared_ptr<ws::ssl::context> ctx = make_shared<ws::ssl::context>(ws::ssl::context::sslv23);

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
#endif

void NetworkClient::onOpenConnection(ws::connection_hdl hdl) {
    cout << TAG << "Connection is opened" << endl;
    chdl = hdl;
    semaphore.release();
}

void NetworkClient::onReceiveMessage(websocketpp::connection_hdl, ws::client::message_ptr msg) {
    if (!onReceiveAudioCallback) {
        return;
    }
    string payload = msg->get_payload();
    vector<uint8_t> audio(payload.begin(), payload.end());
    onReceiveAudioCallback(audio);
}

void NetworkClient::onFailConnection(ws::connection_hdl hdl) {
    cout << TAG << "Connection is failed" << endl;
}

void NetworkClient::onCloseConnection(ws::connection_hdl hdl) {
    cout << TAG << "Connection is closed" << endl;
}

void NetworkClient::sendAudio(vector<uint8_t> data) {
    ws::error_code ec;
    ws::client::connection_ptr con = client.get_con_from_hdl(chdl, ec);
    if (ec) {
        cout << TAG << "Sending audio failed: " << ec.message() << endl;
        return;
    }

    ec = con->send(data.data(), data.size(), websocketpp::frame::opcode::binary);   
    if (ec) {
        cout << TAG << "Sending audio failed: " << ec.message() << endl;
        return;
    }

    // cout << TAG << "Audio sent" << endl;
}

NetworkClient::~NetworkClient() {}
