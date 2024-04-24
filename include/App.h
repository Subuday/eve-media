#include <atomic>
#include <functional>  
#include <pulse/pulseaudio.h>
#include <NetworkClient.h>
#include <MediaClient.h>
#include <Semaphore.h>
#include <mutex>
#include <thread>
#include <queue>

#ifndef MAIN_H
#define MAIN_H

using namespace std; 

class App {
    private:
        atomic<pa_threaded_mainloop*> loop;
        NetworkClient networkClient;
        MediaClient mediaClient;

         atomic<bool> firstCallReceived{false};

    public:
        static App& instance(); 

        App();
        void start();
        MediaClient* getMediaClient();
        ~App();
};

#endif