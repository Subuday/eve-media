#pragma once

#include <atomic>
#include <functional>  
#include <pulse/pulseaudio.h>
#include <NetworkClient.h>
#include <MediaClient.h>
#include <Semaphore.h>
#include <mutex>
#include <thread>
#include <queue>

using namespace std; 

class App {
    private:
        mutex mtx;
        condition_variable cv;
        queue<function<void()>> q;
        atomic<bool> isCancelled;
        
        atomic<pa_threaded_mainloop*> loop;
        NetworkClient networkClient;
        MediaClient mediaClient;

         atomic<bool> firstCallReceived{false};

    public:
        static App& instance(); 

        App();
        void start();
        void run();
        MediaClient* getMediaClient();
        ~App();
};