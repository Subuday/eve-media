#include <NetworkClient.h>
#include <queue>

const string NetworkClient::TAG = "[NetworkClient] ";

NetworkClient::NetworkClient() : ctx(ssl::context::tlsv12_client) {}

void NetworkClient::start() {
    session = make_shared<Session>(ioc, ctx);
    session->run("api.deepgram.com", "443");
    worker = thread([this](){ ioc.run(); });
}

void NetworkClient::setOnReceiveAudioCallback(function<void(vector<uint8_t>)> callback) {
    onReceiveAudioCallback = callback;
}

void NetworkClient::sendAudio(vector<int8_t> data) {
    session->send(data);
}

void NetworkClient::stop() {
    // TODO: Implement
}

NetworkClient::~NetworkClient() {}
