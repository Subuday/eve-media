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
    pa_stream_set_underflow_callback(stream, stream_underflow_callback, nullptr);
    pa_stream_set_write_callback(stream, stream_write_callback, this);
}

void MediaManager::Player::stream_underflow_callback(pa_stream *s, void *userdata) {
    std::cerr << "Player stream underflow occurred!" << std::endl;
}

void MediaManager::Player::stream_write_callback(pa_stream* s, size_t length, void* userdata) {
    // TODO: ADD Logs and null checks and check thread safety
    Player* player = static_cast<Player*>(userdata);
    // if (player == nullptr) {
    //     std::cerr << "Userdata is not set correctly, it's NULL." << std::endl;
    //     return;
    // }

    std::cout << "Player stream requested " << length << " bytes." << std::endl;

    // std::lock_guard<std::mutex> lock(player->bufferMutex);  // Ensure thread safety

    if (player->buffer.empty()) {
        std::cerr << "Player buffer is empty, nothing to write." << std::endl;
        return;
    }

    size_t chunk_size = std::min(length, player->buffer.size());
    const uint8_t* chunk_ptr = player->buffer.data();

    int result = pa_stream_write(s, chunk_ptr, chunk_size, nullptr, 0, PA_SEEK_RELATIVE);
    if (result == 0) {
        std::cout << "Player succeeded to write to stream " << chunk_size << " bytes" << std::endl;
    } else {
        std::cerr << "Player failed to write to stream: " << pa_strerror(result) << std::endl;
        return;
    }

    player->buffer.erase(player->buffer.begin(), player->buffer.begin() + chunk_size);
}

void MediaManager::Player::start() {

}

void MediaManager::Player::write(const vector<uint8_t>& data) {
    // std::cout << "Player buffer is fulfilled!" << std::endl;
    buffer.insert(buffer.end(), data.begin(), data.end());
    //TODO: Handle other states! 

    if (buffer.size() >= 44100) {
        pa_stream_state_t state = pa_stream_get_state(stream);
        switch (state) {
            case PA_STREAM_UNCONNECTED: {
                int result = pa_stream_connect_playback(stream, nullptr, nullptr, PA_STREAM_NOFLAGS, nullptr, nullptr);
                if (result == 0) {
                    std::cout << "Player succeeded to connect stream!" << std::endl;
                } else {
                    std::cerr << "Player failed to connect stream!" << std::endl;
                }
                break;
            }
            case PA_STREAM_CREATING:
                std::cout << "Player stream is being created." << std::endl;
                break;
            case PA_STREAM_READY:
                std::cout << "Player tream is ready for use." << std::endl;
                break;
            case PA_STREAM_FAILED:
                std::cerr << "Player stream has failed." << std::endl;
                break;
            case PA_STREAM_TERMINATED:
                std::cerr << "Player stream has been terminated." << std::endl;
                break;
            default:
                std::cerr << "Player stream is in unknown state." << std::endl;
                break;
        }     
    }

    // pa_stream_write(stream, data.data(), data.size(), nullptr, 0, PA_SEEK_RELATIVE);
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
