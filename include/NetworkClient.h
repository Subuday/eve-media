#pragma once

#include <Session.h>
#include <functional>
#include <thread>
#include <memory>

using namespace std;

class NetworkClient {
private:
    static const string TAG;  

    net::io_context ioc;
    ssl::context ctx;
    thread worker;
    shared_ptr<Session> session;

    function<void(vector<int8_t>)> onReceiveAudioCallback;
public:
    NetworkClient();
    ~NetworkClient();

    void start();
    void setOnReceiveAudioCallback(function<void(vector<int8_t>)> callback);
    void sendAudio(vector<int8_t>& data);
    void stop();
};
