#include <App.h>
#include <iostream>
#include <MediaClient.h>
#include <thread>
#include <pulse/pulseaudio.h>

App& App::instance() {
    static App instance;
    return instance;
}

App::App() {

}

void App::start() {
    //TODO: Handle error
    // Make volitile
    networkClient.prepare();
    mediaClient.prepare();

    networkClient.setOnReceiveAudioCallback([this](vector<uint8_t> audio) {
        mediaClient.write(audio);
    });
    auto t = new thread([this](){
        while (true) {
            auto data = mediaClient.read();
            if (data.size() >= 4096) {
                networkClient.sendAudio(data);
            }
        }
    });


    cout << "App started" << endl;


    t->join();

    //TODO: Clean up of pulse audio
}

MediaClient* App::getMediaClient() {
    return nullptr;
}

App::~App() {}
