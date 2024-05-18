#pragma once

#include <MediaClient.h>
#include <NetworkClient.h>

class SpeakerAudioStreamingUseCase {
    private:
        MediaClient& mediaClient;
        NetworkClient& networkClient;
    public:
        SpeakerAudioStreamingUseCase(MediaClient& mediaClient, NetworkClient& networkClient);
};