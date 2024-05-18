#include <App.h>
#include <iostream>
#include <MediaClient.h>
#include <SignalListenerUseCase.h>
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

App::App() : isCancelled(false), surface(view), manageEyesStateUseCase(view), micAudioStreamingUseCase(mediaClient, networkClient), speakerAudioStreamingUseCase(mediaClient, networkClient) {}

void App::start() {
    //TODO: Handle error
    // Make volitile
    surface.init();

    SignalListenerUseCase::callbacks({
        [this] { this->manageEyesStateUseCase.openEyes(); },
        [this] { this->manageEyesStateUseCase.blink(); },
        [this] { this->manageEyesStateUseCase.think(); },
        [this] { this->manageEyesStateUseCase.speak(); },
        [this] { this->manageEyesStateUseCase.trigger(0); },
        [this] { this->manageEyesStateUseCase.trigger(1); },
        [this] { this->manageEyesStateUseCase.trigger(2); },
        [this] { this->manageEyesStateUseCase.closeEyes(); }
    });
    SignalListenerUseCase sluc;

    MicButtonListenerUseCase::callback([this](bool isPressed) { 
        // TODO: Check thread that is called
        if (isPressed) {
            manageEyesStateUseCase.speak();
        }
    });
    MicButtonListenerUseCase mcblc;

    shutdownListenerUseCase.callback([this] {
        // Check thread that is called
        manageEyesStateUseCase.closeEyes();
    });

    manageEyesStateUseCase.openEyes();

    // networkClient.prepare();
    // mediaClient.prepare();


    cout << "App started" << endl;

    //TODO: Clean up of pulse audio
}

void App::run() {
    start();
    // TODO: Finish unfinished tasks
    while (!isCancelled.load()) {
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
