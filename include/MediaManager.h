#include <pulse/pulseaudio.h>

#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H

class MediaManager {
public:
    MediaManager(pa_context* ctx);
    ~MediaManager(); 
};

#endif