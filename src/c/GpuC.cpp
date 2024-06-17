#include <cstdlib>

#include "Gpu.hpp"
#include "GpuC.h"

#ifdef __cplusplus
extern "C" {
#endif

static Gpu *instance = NULL;

void Gpu_init() {
    instance = new Gpu();
    instance->init();
}

void Gpu_post(uint16_t* buffer) {
    instance->post(buffer);
}

void Gpu_deinit() {

}

#ifdef __cplusplus
}
#endif