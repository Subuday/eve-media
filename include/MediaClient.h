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
        vector<uint8_t> buffer;

        static void streamStateCallback(pa_stream *s, void *userdata);
        static void streamWriteCallback(pa_stream* s, size_t length, void* userdata);
        static void streamWriteFreeCallback(void* p);

        void connectStream();
    public:
        Player(CountDownLatch& countDownLatch, pa_stream* stream);

        void prepare();
        void write(const vector<uint8_t>& data);
    };

    class Recorder {
    private:
        mutex mtx;
        CountDownLatch& countDownLatch;
        pa_stream* stream;
        vector<int8_t> buffer;

        static void streamReadCallback(pa_stream* s, size_t length, void* userdata);

        void connectStream();
    public:
        Recorder(CountDownLatch& countDownLatch, pa_stream* stream);

        void prepare();
        vector<int8_t> read();
    };
};

#endif
