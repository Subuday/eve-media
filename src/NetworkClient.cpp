#include <NetworkClient.h>
#include <queue>

const string NetworkClient::TAG = "[NetworkClient] ";

NetworkClient::NetworkClient() : ctx(ssl::context::tlsv12_client) {}

void NetworkClient::start() {
    session = make_shared<Session>(ioc, ctx);
    session->setOnReadCallback([this](beast::flat_buffer& buffer) {
        vector<int8_t> data(buffer.size());
        copy_n(static_cast<const int8_t*>(buffer.data().data()), buffer.size(), data.begin());
        onReceiveAudioCallback(data);
    });
    // session->run("api.deepgram.com", "443");
    session->run("192.168.31.100", "3000");
    worker = thread([this](){ ioc.run(); });
}

void NetworkClient::sendStartRecording() {
    string msg = "{\"type\":\"start\"}";
    vector<int8_t> bytes(msg.begin(), msg.end());
    session->send(bytes);
}

void NetworkClient::setOnReceiveAudioCallback(function<void(vector<int8_t>)> callback) {
    onReceiveAudioCallback = callback;
}

void NetworkClient::sendStopRecording() {
    string msg = "{\"type\":\"stop\"}";
    vector<int8_t> bytes(msg.begin(), msg.end());
    session->send(bytes);
}

void NetworkClient::sendAudio(vector<int8_t>& data) {
    session->send(data);
}

void NetworkClient::stop() {
    // TODO: Implement
}

NetworkClient::~NetworkClient() {}
