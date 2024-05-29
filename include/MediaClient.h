#include <atomic>
#include <functional>
#include <pulse/pulseaudio.h>
#include <vector>
#include <mutex>
#include <memory>

#include <CountDownLatch.h>

#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H

using namespace std; 

class MediaClient {
public:
    class Player;
    class Recorder;
private:
    static const string TAG;
    
    static pa_threaded_mainloop* _loop;
    
    CountDownLatch countDownLatch;
    unique_ptr<Recorder> _recorder;
    unique_ptr<Player> _player;

    static void contextStateCallback(pa_context *c, void *userdata);
    void onContextReady(pa_context *c);
public:
    MediaClient();
    ~MediaClient(); 

    void prepare();
    Recorder& recorder() const;
    Player& player() const;

    class Player {
        friend class MediaClient;
    private:
        mutex mtx;
        CountDownLatch& countDownLatch;
        pa_stream* stream;
        vector<int8_t> buffer;

        static void streamStateCallback(pa_stream *s, void *userdata);
        static void streamWriteCallback(pa_stream* s, size_t length, void* userdata);
        static void streamWriteFreeCallback(void* p);

        void prepare();
        void connectStream();
    public:
        Player(CountDownLatch& countDownLatch, pa_stream* stream);

        void write(const vector<int8_t>& data);
        void interrupt();
    };

    class Recorder {
        friend class MediaClient;
    private:
        mutex mtx;
        CountDownLatch& countDownLatch;
        pa_stream* stream;
        vector<int8_t> buffer;
        function<void(vector<int8_t>)> onReadCallback;

        static void streamStateCallback(pa_stream *s, void *userdata);
        static void streamSuspendedCallback(pa_stream *s, void *userdata); 
        static void streamReadCallback(pa_stream* s, size_t length, void* userdata);
        static void streamPauseCallback(pa_stream* s, int success, void* userdata);
        void prepare();
        void connectStream();
        vector<int8_t> read(size_t bytes);
    public:
        Recorder(CountDownLatch& countDownLatch, pa_stream* stream);

        void setOnReadCallback(const function<void(vector<int8_t>)> callback);
        void startRecording();
        void stopRecording();
    };
};

#endif
