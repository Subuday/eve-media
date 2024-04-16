#include <iostream>
#include <main.h>
#include <MediaManager.h>
#include <pulse/pulseaudio.h>

void App::context_state_callback(pa_context *c, void *userdata) {
    //TODO: Add logging
    MediaManager* manager = static_cast<MediaManager*>(userdata);
    switch (pa_context_get_state(c)) {
        case PA_CONTEXT_READY: {
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

App::App() {
    //TODO: Handle error
    loop = pa_mainloop_new();

    // TODO: Handle error
    pa_mainloop_api *api = pa_mainloop_get_api(loop);

    pa_context *context = pa_context_new(api, "eve-media");
    manager = new MediaManager(context);
    pa_context_set_state_callback(context, context_state_callback, &manager);
    pa_context_connect(context, nullptr, PA_CONTEXT_NOAUTOSPAWN, nullptr);
}

void App::run() {
    pa_mainloop_run(loop, NULL);
}

App::~App() {
    delete manager;
    pa_mainloop_free(loop);
}

int main(int argc, char *argv[]) {
    App().run();
    return 0;
}