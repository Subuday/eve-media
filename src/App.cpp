#include <App.h>
#include <iostream>
#include <MediaClient.h>
#include <thread>
#include <pulse/pulseaudio.h>

volatile bool programRunning = true;

void MarkProgramQuitting()
{
  programRunning = false;
  // post([]{});
}

App& App::instance() {
    static App instance;
    return instance;
}

App::App() : surface(view), manageEyesStateUseCase(view) {}

void App::start() {
    //TODO: Handle error
    // Make volitile
    surface.init();

    manageEyesStateUseCase.openEyes();

    networkClient.prepare();
    mediaClient.prepare();

    networkClient.setOnReceiveAudioCallback([this](vector<uint8_t> audio) {
        mediaClient.write(audio);
    });
    auto t = new thread([this](){
        //this_thread::sleep_for(chrono::seconds(5));
        while (true) {
            auto data = mediaClient.read();
            if (data.size() != 0) {
                // this_thread::sleep_for(chrono::milliseconds(100));
                networkClient.sendAudio(data);
            }
        }
    });


    cout << "App started" << endl;

    //TODO: Clean up of pulse audio
}

void App::run() {
    start();
    while (isCancelled.load()) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [this] { return !q.empty(); });
        function<void()> cb = q.front();
        q.pop();
        lock.unlock();

        cb();
    }
}

MediaClient* App::getMediaClient() {
    return nullptr;
}

App::~App() {}
