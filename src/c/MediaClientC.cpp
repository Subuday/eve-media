#pragma once

#include "MediaClient.h"
#include "MediaClientC.h"

#ifdef __cplusplus
extern "C" {
#endif

static MediaClient *instance = NULL;

void media_init() {
    instance = new MediaClient();
}

void media_start() {
    instance->prepare();
}

void player_write(int8_t* data, int size) {
    vector<int8_t> bytes(data, data + size);
    instance->player().write(bytes);
}

void recorder_set_on_read_callback(void* ctx, void (*callback)(int8_t*, int, void*)) {
    instance->recorder().setOnReadCallback([ctx, callback](vector<int8_t> data) {
        callback(data.data(), data.size(), ctx);
    });
}

void recorder_start() {
    instance->recorder().startRecording();
}

void recorder_stop() {
    instance->recorder().stopRecording();
}

#ifdef __cplusplus
}
#endif