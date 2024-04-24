#include <atomic>
#include <pulse/pulseaudio.h>
#include <vector>
#include <mutex>

#include <Semaphore.h>

#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H

using namespace std; 

class MediaClient {
public:
    class Player;
    class Recorder;
private:
    static pa_threaded_mainloop* _loop;
    
    Semaphore semaphore;
    Recorder* recorder = nullptr;
    Player* player = nullptr;

    static void contextStateCallback(pa_context *c, void *userdata);
    void onContextReady(pa_context *c);
public:
    static pa_threaded_mainloop* loop();

    MediaClient();
    ~MediaClient(); 

    void prepare();
    vector<uint8_t> read() const;
    void write(const vector<uint8_t>& data);

    class Player {
    private:
        mutex mtx;
        pa_stream* stream;
        atomic<bool> isUnderflow;
        vector<uint8_t> buffer;

        static void streamStateCallback(pa_stream *s, void *userdata);
        static void stream_underflow_callback(pa_stream *s, void *userdata);
        static void stream_write_callback(pa_stream* s, size_t length, void* userdata);
        static void stream_write_free_callback(void* p);

        void connectStreamIfBufferIsSufficient();
        void handleStreamReadyState();
        bool writeStream(size_t chunk_size);
    public:
        Player(pa_stream* stream);

        void start();
        void write(const vector<uint8_t>& data);
    };

    class Recorder {
    private:
        mutex mtx;
        pa_stream* stream;
        vector<uint8_t> buffer;

        static void stream_read_callback(pa_stream* s, size_t length, void* userdata);

        void connectStream();
    public:
        Recorder(pa_stream* stream);

        void start();
        vector<uint8_t> read();
    };
};

#endif
