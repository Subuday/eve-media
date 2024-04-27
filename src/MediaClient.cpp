#include <App.h>
#include <algorithm> 
#include <chrono>
#include <iostream>
#include <thread>
#include "Semaphore.h"
#include "MediaClient.h"
#include <vector>
#include <pulse/pulseaudio.h>

pa_threaded_mainloop* MediaClient::_loop = nullptr;

pa_threaded_mainloop* MediaClient::loop() {
    return MediaClient::_loop;
}

MediaClient::MediaClient() : countDownLatch(2) {}

void MediaClient::contextStateCallback(pa_context *c, void *userdata) {
    //TODO: Add logging
    MediaClient* mediaClient = static_cast<MediaClient*>(userdata);
    switch (pa_context_get_state(c)) {
        case PA_CONTEXT_READY: {
            mediaClient->onContextReady(c);
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

void MediaClient::prepare() {
    pa_threaded_mainloop* loop = pa_threaded_mainloop_new();
    MediaClient::_loop = loop;

    //TODO: Handle error
    pa_threaded_mainloop_start(loop);

    pa_threaded_mainloop_lock(loop);

    // TODO: Handle error
    pa_mainloop_api *api = pa_threaded_mainloop_get_api(loop);

    pa_context *context = pa_context_new(api, "eve-project");
    pa_context_set_state_callback(context, contextStateCallback, this);
    pa_context_connect(context, nullptr, PA_CONTEXT_NOFLAGS, nullptr);

    pa_threaded_mainloop_unlock(loop);

    countDownLatch.await();
}

void MediaClient::onContextReady(pa_context *c) {
    //Add logging and error handling
    cout << "Context is ready!" << endl;

    pa_sample_spec ss;
    ss.format = PA_SAMPLE_S16LE;
    ss.channels = 2;
    ss.rate = 44100;

    pa_sample_spec pss;
    pss.format = PA_SAMPLE_S16LE;
    pss.channels = 1;
    pss.rate = 16000;
    
    pa_stream* rs = pa_stream_new(c, "eva-media-recorder", &ss, nullptr);
    pa_stream* ps = pa_stream_new(c, "eva-media-player", &pss, nullptr);

    recorder = new Recorder(countDownLatch, rs);
    player = new Player(countDownLatch, ps);

    countDownLatch.countDown();
    countDownLatch.countDown();
}

vector<int16_t> MediaClient::read() const {
    if (recorder != nullptr) {
        return recorder->read();
    } else {
        cerr << "Recorder is not initialised!" << endl;
        vector<int16_t> data;
        return data;
    }
}

void MediaClient::write(const vector<uint8_t>& data) {
    if (player != nullptr) {
        player->write(data);
    } else {
        std::cerr << "Player is not initialised!" << std::endl;
    }
}

MediaClient::Player::Player(CountDownLatch& countDownLatch, pa_stream* stream) : countDownLatch(countDownLatch), stream(stream) {
    pa_stream_set_state_callback(stream, streamStateCallback, nullptr);
    pa_stream_set_underflow_callback(stream, stream_underflow_callback, this);
    pa_stream_set_write_callback(stream, stream_write_callback, this);
}

void MediaClient::Player::streamStateCallback(pa_stream *s, void *userdata) {
    switch (pa_stream_get_state(s)) {
        case PA_STREAM_READY: {
            const pa_buffer_attr *attr = pa_stream_get_buffer_attr(s);
            if (attr) {
                std::cout << "Ready to query buffer attributes:" << std::endl;
                std::cout << "MaxLength = " << attr->maxlength << std::endl;
                std::cout << "MaxLength = " << attr->maxlength << std::endl;
                std::cout << "TargetLength = " << attr->tlength << std::endl;
                std::cout << "PreBuf = " << attr->prebuf << std::endl;
                std::cout << "MinReq = " << attr->minreq << std::endl;
            } else {
                std::cerr << "Player failed to get buffer attributes" << std::endl;
            }
            break;
        }
        default:
            break;
    }
}

void MediaClient::Player::stream_underflow_callback(pa_stream *s, void *userdata) {
    Player* player = static_cast<Player*>(userdata);
    player->isUnderflow = true;
    cerr << "Player stream underflow occurred!" << endl;
    //TODO: Add trigering of writing
}

void MediaClient::Player::stream_write_callback(pa_stream* s, size_t length, void* userdata) {
    // TODO: ADD Logs and null checks
    Player* player = static_cast<Player*>(userdata);

    cout << "Player stream requested " << length << " bytes." << endl;
    size_t bufferSize;
    {
        lock_guard<mutex> lock(player->mtx);
        bufferSize = player->buffer.size();
    }

    if (bufferSize == 0) {
        cerr << "Player buffer is empty, nothing to write." << endl;
        return;
    }

    size_t chunkSize = min(bufferSize, length);
    player->writeStream(chunkSize);
}

void MediaClient::Player::stream_write_free_callback(void* p) {
    delete[] static_cast<uint8_t*>(p);
}

bool MediaClient::Player::writeStream(size_t chunkSize) {
    if (chunkSize == 0) {
        return false;
    }

    uint8_t* chunk = new uint8_t[chunkSize];
    {
        lock_guard<mutex> lock(mtx);
        if (buffer.size() < chunkSize) {
            std::cerr << "Player has insufficient data in buffer." << std::endl;
            return false;
        }
        copy_n(buffer.begin(), chunkSize, chunk);
    }

    int result = pa_stream_write_ext_free(stream, chunk, chunkSize, stream_write_free_callback, chunk, 0, PA_SEEK_RELATIVE);

    if (result == 0) {
        {
            lock_guard<mutex> lock(mtx);
            buffer.erase(buffer.begin(), buffer.begin() + chunkSize);
        }
        std::cout << "Player succeeded to write to stream " << chunkSize << " bytes" << std::endl;
    } else {
        std::cerr << "Player failed to write to stream: " << pa_strerror(result) << std::endl;
    }

    return result == 0;
}

void MediaClient::Player::start() {

}

void MediaClient::Player::write(const vector<uint8_t>& data) {
    // std::cout << "Player buffer is fulfilled!" << std::endl;
    {
        lock_guard<mutex> lock(mtx);
        buffer.insert(buffer.end(), data.begin(), data.end());
    }

    pa_threaded_mainloop* loop = MediaClient::loop();
    pa_threaded_mainloop_lock(loop);
    pa_stream_state_t state = pa_stream_get_state(stream);
    pa_threaded_mainloop_unlock(loop);

    switch (state) {
        case PA_STREAM_UNCONNECTED:
            connectStreamIfBufferIsSufficient();
            break;
        case PA_STREAM_CREATING:
            cout << "Player stream is being created." << endl;
            break;
        case PA_STREAM_READY:
            handleStreamReadyState();
            break;
        case PA_STREAM_FAILED:
            cerr << "Player stream has failed." << endl;
            break;
        case PA_STREAM_TERMINATED:
            cerr << "Player stream has been terminated." << endl;
            break;
        default:
            cerr << "Player stream is in unknown state." << endl;
            break;
    }   
}

void MediaClient::Player::connectStreamIfBufferIsSufficient() {
    cout << "Player tries connecting stream!" << endl;

    pa_threaded_mainloop* loop = MediaClient::loop();

    pa_threaded_mainloop_lock(loop);

    pa_stream_state_t state = pa_stream_get_state(stream);
    if (state != PA_STREAM_UNCONNECTED) {
        pa_threaded_mainloop_unlock(loop);
        return;
    }

    size_t bufferSize;
    {
        lock_guard<mutex> lock(mtx);
        bufferSize = buffer.size();
    }

    if (bufferSize < 44100) {
        pa_threaded_mainloop_unlock(loop);
        return;
    }

    int result = pa_stream_connect_playback(stream, nullptr, nullptr, PA_STREAM_NOFLAGS, nullptr, nullptr);
    if (result == 0) {
        cout << "Player succeeded to connect stream!" << endl;
    } else {
        cerr << "Player failed to connect stream!" << endl;
    }

    pa_threaded_mainloop_unlock(loop);
}

void MediaClient::Player::handleStreamReadyState() {
    if (isUnderflow) {
        pa_threaded_mainloop* loop = MediaClient::loop();

        pa_threaded_mainloop_lock(loop);

        if (!isUnderflow) {
            pa_threaded_mainloop_unlock(loop);
            return;
        }

        size_t bufferSize;
        {
            lock_guard<mutex> lock(mtx);
            bufferSize = buffer.size();
        }
        if (bufferSize < 44100 * 10) {
            pa_threaded_mainloop_unlock(loop);
            return;
        }

        pa_stream_state_t state = pa_stream_get_state(stream);
        if (state != PA_STREAM_READY) {
            pa_threaded_mainloop_unlock(loop);
            return;
        }

        size_t streamWritableSize = pa_stream_writable_size(stream);
        if (streamWritableSize == (size_t) - 1) {
            cerr << "Player failed to  query writable size during underflow handling." << endl;
            pa_threaded_mainloop_unlock(loop);
            return;
        }

        size_t chunkSize = std::min(streamWritableSize, bufferSize);

        bool result = writeStream(chunkSize);

        if (result) {
            isUnderflow = true;
        }

        pa_threaded_mainloop_unlock(loop);
    }
}

MediaClient::Recorder::Recorder(CountDownLatch& countDownLatch, pa_stream* stream) : countDownLatch(countDownLatch), stream(stream) {
    pa_stream_set_read_callback(stream, stream_read_callback, this);
}

void MediaClient::Recorder::stream_read_callback(pa_stream* stream, size_t nbytes, void* userdata) {
    Recorder* recorder = static_cast<Recorder*>(userdata);
    
    const void* chunk;
    int result = pa_stream_peek(stream, &chunk, &nbytes);
    if (result < 0) {
        cerr << "Recorder failed to read data from stream" << endl;
        return;
    }

    if (chunk && nbytes > 0) {
        {
            lock_guard<mutex> lock(recorder->mtx);
            const int16_t* chunkPtr = static_cast<const int16_t*>(chunk);
            // cout << "Recorder read " << nbytes << " bytes." << endl;
            recorder->buffer.insert(recorder->buffer.end(), chunkPtr, chunkPtr + (nbytes / sizeof(int16_t)));
        }
    }

    pa_stream_drop(stream);
}

void MediaClient::Recorder::start() {

}

vector<int16_t> MediaClient::Recorder::read() {
    pa_threaded_mainloop* loop = MediaClient::loop();
    pa_threaded_mainloop_lock(loop);
    pa_stream_state_t state = pa_stream_get_state(stream);
    pa_threaded_mainloop_unlock(loop);

    switch (state) {
        case PA_STREAM_UNCONNECTED:
            connectStream();
            break;
        case PA_STREAM_CREATING:
            // cout << "Recorder stream is being created." << endl;
            break;
        case PA_STREAM_READY:
            break;
        case PA_STREAM_FAILED:
            cerr << "Recorder stream has failed." << endl;
            break;
        case PA_STREAM_TERMINATED:
            cerr << "Recorder stream has been terminated." << endl;
            break;
        default:
            cerr << "Recorder stream is in unknown state." << endl;
            break;
    }

    const size_t chunkSize = 2048;
    vector<int16_t> chunk;
    {
        lock_guard<mutex> lock(mtx);
        if (buffer.size() >= chunkSize) {
            chunk.assign(buffer.begin(), buffer.begin() + chunkSize);
            buffer.erase(buffer.begin(), buffer.begin() + chunkSize);
        }
    }
    
    return chunk;
}

void MediaClient::Recorder::connectStream() {
    pa_threaded_mainloop* loop = MediaClient::loop();

    pa_threaded_mainloop_lock(loop);

    pa_stream_state_t state = pa_stream_get_state(stream);
    if (state != PA_STREAM_UNCONNECTED) {
        pa_threaded_mainloop_unlock(loop);
        return;
    }

    int result = pa_stream_connect_record(stream, nullptr, nullptr, PA_STREAM_NOFLAGS);
    if (result == 0) {
        cout << "Recorder succeeded to connect stream!" << endl;
    } else {
        cerr << "Recorder failed to connect stream!" << endl;
    }

    pa_threaded_mainloop_unlock(loop);
}

MediaClient::~MediaClient() {}
