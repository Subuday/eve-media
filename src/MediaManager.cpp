#include <App.h>
#include <algorithm> 
#include <chrono>
#include <iostream>
#include <thread>
#include "MediaManager.h"
#include <vector>

MediaManager::MediaManager() {}

MediaManager::~MediaManager() {
    delete recorder;
    delete player;
}

void MediaManager::prepare(pa_context* ctx) {
    //Add logging
    std::cout << "Context is ready!" << std::endl;

    pa_sample_spec ss;
    ss.format = PA_SAMPLE_S16LE;
    ss.channels = 2;
    ss.rate = 44100;
    
    pa_stream* rs = pa_stream_new(ctx, "eva-media-recorder", &ss, nullptr);
    pa_stream* ps = pa_stream_new(ctx, "eva-media-player", &ss, nullptr);
    if (!rs || !ps) {
       return;
    }

    recorder = new Recorder(rs);
    player = new Player(ps);

    std::cout << "Streams are created!" << std::endl;
}

vector<uint8_t> MediaManager::read() const {
    std::cout << "Read data!" << std::endl;
    vector<uint8_t> data;
    return data;
}

void MediaManager::write(const vector<uint8_t>& data) {
    if (player != nullptr) {
        player->write(data);
    } else {
        std::cerr << "Player is not initialised!" << std::endl;
    }
}

MediaManager::Player::Player(pa_stream* stream) : stream(stream) {
    pa_stream_set_state_callback(stream, streamStateCallback, nullptr);
    pa_stream_set_underflow_callback(stream, stream_underflow_callback, this);
    pa_stream_set_write_callback(stream, stream_write_callback, this);
}

void MediaManager::Player::streamStateCallback(pa_stream *s, void *userdata) {
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

void MediaManager::Player::stream_underflow_callback(pa_stream *s, void *userdata) {
    Player* player = static_cast<Player*>(userdata);
    player->isUnderflow = true;
    cerr << "Player stream underflow occurred!" << endl;
    //TODO: Add trigering of writing
}

void MediaManager::Player::stream_write_callback(pa_stream* s, size_t length, void* userdata) {
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

void MediaManager::Player::stream_write_free_callback(void* p) {
    delete[] static_cast<uint8_t*>(p);
}

bool MediaManager::Player::writeStream(size_t chunkSize) {
    if (chunkSize < 3072) { // Minimum number of request bytes
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

void MediaManager::Player::start() {

}

void MediaManager::Player::write(const vector<uint8_t>& data) {
    // std::cout << "Player buffer is fulfilled!" << std::endl;
    {
        lock_guard<mutex> lock(mtx);
        buffer.insert(buffer.end(), data.begin(), data.end());
    }

    pa_threaded_mainloop* loop = App::instance().getLoop();
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

void MediaManager::Player::connectStreamIfBufferIsSufficient() {
    pa_threaded_mainloop* loop = App::instance().getLoop();

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

    if (bufferSize < 44100 * 10) {
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

void MediaManager::Player::handleStreamReadyState() {
    if (isUnderflow) {
        pa_threaded_mainloop* loop = App::instance().getLoop();

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

MediaManager::Recorder::Recorder(pa_stream* stream) : stream(stream) {
    pa_stream_set_read_callback(stream, stream_read_callback, nullptr);
}

void MediaManager::Recorder::stream_read_callback(pa_stream* stream, size_t nbytes, void* userdata) {
    // const void* data;
    // if (pa_stream_peek(s, &data, &length) < 0) {
    //     std::cerr << "Failed to read data from stream" << std::endl;
    //     return;
    // }

    // if (data && length > 0) {
    //     out.write(reinterpret_cast<const char*>(data), length);
    // }

    // // Here you can process the data, but for simplicity, we're just dropping it.
    // // For example, you could write it to a file or process the audio data.

    // pa_stream_drop(s); // Must be called after `pa_stream_peek()`.
}

void MediaManager::Recorder::start() {
    // TODO: Log error and cleaning
    int result = pa_stream_connect_record(stream, nullptr, nullptr, PA_STREAM_NOFLAGS);
    if (result < 0) {
        std::cerr << "Recorder failed to connect stream!" << std::endl;
    }
}

vector<uint8_t> MediaManager::Recorder::read() {
    vector<uint8_t> data;
    return data;
}
