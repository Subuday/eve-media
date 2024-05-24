#pragma once

#include <MediaClient.h>
#include <NetworkClient.h>

using namespace std;

class SpeakerAudioStreamingUseCase {
    private:
        MediaClient& mediaClient;
        NetworkClient& networkClient;
        function<void()> onStreamingStartCallback;
        function<void()> onStreamingStopCallback;
    public:
        SpeakerAudioStreamingUseCase(MediaClient& mediaClient, NetworkClient& networkClient);
        void setOnStreamingStartCallback(function<void()> cb);
        void setOnStreamingStopCallback(function<void()> cb);
        void interrupt();
};