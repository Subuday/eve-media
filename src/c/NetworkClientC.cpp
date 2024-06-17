#pragma once

#include "NetworkClient.h"
#include "NetworkClientC.h"

#ifdef __cplusplus
extern "C" {
#endif

static NetworkClient *instance = NULL;

void net_init() {
    instance = new NetworkClient();
}

void net_start() {
    instance->start();
}

void net_stop() {
    instance->stop();
}

void net_send_start_recording() {
    instance->sendStartRecording();
}

void net_send_stop_recording() {
    instance->sendStopRecording();
}

void net_send_audio(int8_t* data, int size) {
    vector<int8_t> bytes(data, data + size);
    instance->sendAudio(bytes);
}

void net_set_on_receive_audio_callback(void* ctx, void (*callback)(int8_t*, int, void*)) {
    instance->setOnReceiveAudioCallback([ctx, callback](vector<int8_t> data) {
        callback(data.data(), data.size(), ctx);
    });
}

#ifdef __cplusplus
}
#endif