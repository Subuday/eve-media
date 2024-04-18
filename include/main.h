#include <pulse/pulseaudio.h>
#include <MediaManager.h>
#include <thread>

#ifndef MAIN_H
#define MAIN_H

class App {
    private:
        std::thread* appThread;
        static void context_state_callback(pa_context *c, void *userdata);
        void loop();
    public:
        MediaManager manager;

        App();
        void start();
        void join();
        ~App();
};

#endif