#include <pulse/pulseaudio.h>
#include <vector>

#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H

using std::vector; 

class MediaManager {
public:
    class Player;
    class Recorder;
private:
    Recorder* recorder = nullptr;
    Player* player = nullptr;
public:
    MediaManager();
    ~MediaManager(); 

    void prepare(pa_context* ctx);
    vector<uint8_t> read() const;
    void write(const vector<uint8_t>& data);

    class Player {
    private:
        pa_stream* stream;
        vector<uint8_t> buffer;

        static void stream_underflow_callback(pa_stream *s, void *userdata);
        static void stream_write_callback(pa_stream* s, size_t length, void* userdata);
    public:
        Player(pa_stream* stream);

        void start();
        void write(const vector<uint8_t>& data);
    };

    class Recorder {
    private:
        pa_stream* stream;

        static void stream_read_callback(pa_stream* s, size_t length, void* userdata);
    public:
        Recorder(pa_stream* stream);

        void start();
        vector<uint8_t> read();
    };
};

#endif
