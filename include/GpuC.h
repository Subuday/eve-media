#pragma once

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

void Gpu_init();
void Gpu_post(uint16_t* buffer);
void Gpu_deinit();

#ifdef __cplusplus
}
#endif