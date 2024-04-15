#include <iostream>
#include "MediaManager.h"

MediaManager::MediaManager(pa_context* ctx) : ctx(ctx) {

}

MediaManager::~MediaManager() {

}

void MediaManager::prepare() {
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

    Recorder recorder(rs);
    Player player(ps);

    std::cout << "Streams are created!" << std::endl;
}

MediaManager::Player::Player(pa_stream* stream) : stream(stream) {
    pa_stream_set_read_callback(stream, stream_write_callback, nullptr);
}

void MediaManager::Player::stream_write_callback(pa_stream* s, size_t length, void* userdata) {
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

void MediaManager::Player::start() {
    int result = pa_stream_connect_record(stream, nullptr, nullptr, PA_STREAM_NOFLAGS);
    if (result < 0) {
        std::cerr << "Player failed to connect stream!" << std::endl;
    }
}

MediaManager::Recorder::Recorder(pa_stream* stream) : stream(stream) {
    pa_stream_set_write_callback(stream, stream_read_callback, nullptr);
}

void MediaManager::Recorder::stream_read_callback(pa_stream* stream, size_t nbytes, void* userdata) {
    // std::vector<uint8_t> buffer(nbytes);
    // in.read(reinterpret_cast<char*>(buffer.data()), nbytes);
    // size_t data_read = in.gcount();

    // std::cout << "Write callback is called!." << std::endl;

    // if (data_read > 0) {
    //     pa_stream_write(stream, buffer.data(), data_read, nullptr, 0, PA_SEEK_RELATIVE);
    // }
}

void MediaManager::Recorder::start() {
    // TODO: Log error and cleaning
    int result = pa_stream_connect_record(stream, nullptr, nullptr, PA_STREAM_NOFLAGS);
    if (result < 0) {
        std::cerr << "Recorder failed to connect stream!" << std::endl;
    }
}