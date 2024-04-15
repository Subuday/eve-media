#include <pulse/pulseaudio.h>

#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H

class MediaManager {
private:
    pa_context* ctx;
public:
    MediaManager(pa_context* ctx);
    ~MediaManager(); 

    void prepare();

    class Player {
    private:
        pa_stream* stream;

        void start();
        static void stream_write_callback(pa_stream* s, size_t length, void* userdata);
    public:
        Player(pa_stream* stream);
    };

    class Recorder {
    private:
        pa_stream* stream;

        void start();
        static void stream_read_callback(pa_stream* s, size_t length, void* userdata);
    public:
        Recorder(pa_stream* stream);
    };
};

#endif