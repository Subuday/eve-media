#include <App.h>
#include <iostream>
#include <MediaManager.h>
#include <thread>
#include <pulse/pulseaudio.h>

void App::context_state_callback(pa_context *c, void *userdata) {
    //TODO: Add logging
    MediaManager* mediaManager = static_cast<MediaManager*>(userdata);
    switch (pa_context_get_state(c)) {
        case PA_CONTEXT_READY: {
            mediaManager->prepare(c);
            break;
        }
        case PA_CONTEXT_FAILED:
        case PA_CONTEXT_TERMINATED:
            std::cerr << "Context failed or was terminated" << std::endl;
            // Perhaps signal your application to exit or attempt to reconnect
            break;
        default:
            std::cerr << "Context state was ignored" << std::endl;
            // Other state changes can be handled or ignored
            break;
    }
}

App& App::instance() {
    static App instance;
    return instance;
}

App::App() {}

void App::start() {
    //TODO: Handle error
    // Make volitile

    loop.store(pa_threaded_mainloop_new());
    mediaManager.store(new MediaManager());

    pa_threaded_mainloop* loop = getLoop();

    //TODO: Handle error
    pa_threaded_mainloop_start(loop);

    pa_threaded_mainloop_lock(loop);

    // TODO: Handle error
    pa_mainloop_api *api = pa_threaded_mainloop_get_api(loop);

    pa_context *context = pa_context_new(api, "eve-media");
    pa_context_set_state_callback(context, context_state_callback, getMediaManager());
    pa_context_connect(context, nullptr, PA_CONTEXT_NOFLAGS, nullptr);

    pa_threaded_mainloop_unlock(loop);

    //TODO: Clean up of pulse audio
}

pa_threaded_mainloop* App::getLoop() {
    return loop.load();
}

MediaManager* App::getMediaManager() {
    return mediaManager.load();
}

App::~App() {}
