#include <pulse/pulseaudio.h>
#include <vector>

#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H

using std::vector; 

class MediaManager {
private:
    pa_context* ctx;
public:
    MediaManager(pa_context* ctx);
    ~MediaManager(); 

    void prepare();
    vector<uint8_t> read() const;
    void write(const vector<uint8_t>& data);

    class Player {
    private:
        pa_stream* stream;

        void start();
        static void stream_write_callback(pa_stream* s, size_t length, void* userdata);
    public:
        Player(pa_stream* stream);

        void write(const vector<uint8_t>& data);
    };

    class Recorder {
    private:
        pa_stream* stream;

        void start();
        static void stream_read_callback(pa_stream* s, size_t length, void* userdata);
    public:
        Recorder(pa_stream* stream);

        vector<uint8_t> read();
    };
};

#endif
