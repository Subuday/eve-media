#pragma once

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

void net_init();
void net_start();
void net_stop();
void net_send_start_recording();
void net_send_stop_recording();
void net_send_audio(int8_t* data, int size);
void net_set_on_receive_audio_callback(void* ctx, void (*callback)(int8_t*, int, void*));

#ifdef __cplusplus
}
#endif