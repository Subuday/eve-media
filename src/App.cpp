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

App::App() : isCancelled(false), surface(view), mediaClient(), networkClient(), manageEyesStateUseCase(view), micAudioStreamingUseCase(mediaClient, networkClient), speakerAudioStreamingUseCase(mediaClient, networkClient) {}

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

    shutdownListenerUseCase.callback([this] {
        // Check thread that is called
        manageEyesStateUseCase.closeEyes();
    });

    networkClient.start();
    mediaClient.prepare();

    manageEyesStateUseCase.openEyes();

    speakerAudioStreamingUseCase.setOnStreamingStartCallback([this] {
        manageEyesStateUseCase.speak();
    });
    speakerAudioStreamingUseCase.setOnStreamingStopCallback([this] {
        manageEyesStateUseCase.blink();
    });

    micAudioStreamingUseCase.setOnStreamingStartCallback([this] {
        manageEyesStateUseCase.think();
    });
    micAudioStreamingUseCase.setOnStreamingStopCallback([this] {
        manageEyesStateUseCase.blink();
    });
    micAudioStreamingUseCase.prepare();    

    buttonClient.setOnButtonPressCallback([this](bool isPressed) {
        if (isPressed) {
            micAudioStreamingUseCase.startStreaming();
        } else {
            micAudioStreamingUseCase.stopStreaming();
        }
    });
    buttonClient.prepare();


    cout << "App started - " << &networkClient << endl;

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
