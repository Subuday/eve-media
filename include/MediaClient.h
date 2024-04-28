#include <atomic>
#include <pulse/pulseaudio.h>
#include <vector>
#include <mutex>

#include <CountDownLatch.h>

#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H

using namespace std; 

class MediaClient {
public:
    class Player;
    class Recorder;
private:
    static pa_threaded_mainloop* _loop;
    
    CountDownLatch countDownLatch;
    Recorder* recorder = nullptr;
    Player* player = nullptr;

    static void contextStateCallback(pa_context *c, void *userdata);
    void onContextReady(pa_context *c);
public:
    static pa_threaded_mainloop* loop();

    MediaClient();
    ~MediaClient(); 

    void prepare();
    vector<int8_t> read() const;
    void write(const vector<uint8_t>& data);

    class Player {
    private:
        mutex mtx;
        CountDownLatch& countDownLatch;
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
        Player(CountDownLatch& countDownLatch, pa_stream* stream);

        void start();
        void write(const vector<uint8_t>& data);
    };

    class Recorder {
    private:
        mutex mtx;
        CountDownLatch& countDownLatch;
        pa_stream* stream;
        vector<int8_t> buffer;

        static void stream_read_callback(pa_stream* s, size_t length, void* userdata);

        void connectStream();
    public:
        Recorder(CountDownLatch& countDownLatch, pa_stream* stream);

        void start();
        vector<int8_t> read();
    };
};

#endif
