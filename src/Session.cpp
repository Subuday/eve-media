#include <Session.h>

const string Session::TAG = "[Session] ";

Session::Session(net::io_context& ioc, ssl::context& ctx): 
    resolver(net::make_strand(ioc)), 
    #ifndef NO_TLS
    ws(net::make_strand(ioc), ctx),
    #else
    ws(net::make_strand(ioc)),
    #endif
    strand(net::make_strand(ioc)) {
        ws.binary(true);
    }

Session::~Session() {}

void Session::run(char const* host, char const* port) {
    // Save these for later
    this->host = host;

    // Look up the domain name
    resolver.async_resolve(
        host,
        port,
        beast::bind_front_handler(&Session::onResolve, shared_from_this())
    );
}

void Session::send(vector<int8_t>& data) {
    if(!strand.running_in_this_thread()) {
        return net::post(
            strand,
            bind(&Session::send, this->shared_from_this(), data)
        );
    }

    // cout << TAG << "Sending: " << data.size() << " bytes" << endl;

    q.push(data);

    // Are we already writing?
    if(q.size() > 1) {
        return;
    }
    
    ws.async_write(
        net::buffer(q.front()),
        bind(
            &Session::onWrite,
            shared_from_this(),
            std::placeholders::_1,
            std::placeholders::_2
        )
    );
}

void Session::onResolve(beast::error_code ec, tcp::resolver::results_type results) {
    if (ec) {
        cout << TAG << "Resolve error: " << ec.message() << endl;
        return;
    }

    beast::get_lowest_layer(ws).async_connect(
        results,
        beast::bind_front_handler(
            &Session::onConnect,
            shared_from_this()
        )
    );
}

void Session::onConnect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep) {
    if (ec) {
        cout << TAG << "Connect error: " << ec.message() << endl;
        return;
    }

    #ifndef NO_TLS
    if (!SSL_set_tlsext_host_name(ws.next_layer().native_handle(), host.c_str())) {
        ec = beast::error_code(static_cast<int>(::ERR_get_error()), net::error::get_ssl_category());
        return;
    }
    #endif

    // Update the host_ string. This will provide the value of the
    // Host HTTP header during the WebSocket handshake.
    // See https://tools.ietf.org/html/rfc7230#section-5.4
    host += ':' + std::to_string(ep.port());
    
    #ifndef NO_TLS
    // Perform the SSL handshake
    ws.next_layer().async_handshake(
        ssl::stream_base::client,
        beast::bind_front_handler(
            &Session::onSslHandshake,
            shared_from_this()
        )
    );
    #else
    ws.async_handshake(
        host,
        "/v1/listen?encoding=linear16&sample_rate=44100&channels=2&model=nova-2",
        beast::bind_front_handler(
            &Session::onHandshake,
            shared_from_this()
        )
    );
    #endif
}

#ifndef NO_TLS
void Session::onSslHandshake(beast::error_code ec) {
    if (ec) {
        cout << TAG << "SSL handshake error: " << ec.message() << endl;
        return;
    }

    // Turn off the timeout on the tcp_stream, because
    // the websocket stream has its own timeout system.
    beast::get_lowest_layer(ws).expires_never();

    // Set suggested timeout settings for the websocket
    ws.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));

    ws.set_option(websocket::stream_base::decorator([](websocket::request_type& req) {
        req.set(http::field::authorization, "Token ");
    }));

    // Perform the websocket handshake
    ws.async_handshake(
        host,
        "/v1/listen?encoding=linear16&sample_rate=44100&channels=2&model=nova-2",
        beast::bind_front_handler(
            &Session::onHandshake,
            shared_from_this()
        )
    );
}
#endif

void Session::onHandshake(beast::error_code ec) {
    if (ec) {
        cout << TAG << "Handshake error: " << ec.message() << endl;
        return;
    }

    cout << TAG << "Handshake successful" << endl;

    ws.async_read(
        buffer,
        beast::bind_front_handler(
            &Session::onRead,
            shared_from_this()
        )
    );
}

void Session::onRead(beast::error_code ec, size_t bytes_transferred) {
    if (ec) {
        cout << TAG << "Read error: " << ec.message() << endl;
        return;
    }

    string message = beast::buffers_to_string(buffer.data());
    std::cout << TAG << "Received: " << message << std::endl;

    buffer.consume(buffer.size());

    ws.async_read(
        buffer,
        beast::bind_front_handler(
            &Session::onRead,
            shared_from_this()
        )
    );
}

void Session::onWrite(beast::error_code ec, std::size_t) {
    if (ec) {
        cout << TAG << "Write error: " << ec.message() << endl;
        return;
    }

    q.pop();

    if(!q.empty()) {
        ws.async_write(
            net::buffer(q.front()),
            bind(
                &Session::onWrite,
                shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2
            )
        );
    }
}