#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void media_init();
void media_start();

void player_write(int8_t* data, int size);

void recorder_start();
void recorder_stop();
void recorder_set_on_read_callback(void* ctx, void (*callback)(int8_t*, int, void*));

#ifdef __cplusplus
}
#endif