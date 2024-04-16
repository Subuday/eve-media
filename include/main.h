#include <pulse/pulseaudio.h>
#include <MediaManager.h>

#ifndef MAIN_H
#define MAIN_H

class App {
    private:
        pa_mainloop* loop;

        static void context_state_callback(pa_context *c, void *userdata);
    public:
        MediaManager* manager = nullptr;

        App();
        void run();
        ~App();
};

#endif