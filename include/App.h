#include <atomic>
#include <functional>  
#include <pulse/pulseaudio.h>
#include <NetworkClient.h>
#include <MediaManager.h>
#include <mutex>
#include <thread>
#include <queue>

#ifndef MAIN_H
#define MAIN_H

using namespace std; 

class App {
    private:
        atomic<pa_threaded_mainloop*> loop;
        atomic<NetworkClient*> networkClient;
        atomic<MediaManager*> mediaManager;

        static void context_state_callback(pa_context *c, void *userdata);

    public:
        static App& instance(); 

        App();
        void start();
        pa_threaded_mainloop* getLoop();
        MediaManager* getMediaManager();
        ~App();
};

#endif