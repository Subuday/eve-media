#pragma once

#include <atomic>
#include <functional>
#include <MediaClient.h>
#include <NetworkClient.h>
#include <Timer.h>
#include <thread>

using namespace std;

class MicAudioStreamingUseCase {
    private:
        static const string TAG;

        MediaClient& mediaClient;
        NetworkClient& networkClient;
        Timer timer;
        atomic<bool> isStreaming;
        function<void()> onStreamingStartCallback;
        function<void()> onStreamingStopCallback;
    public:
        MicAudioStreamingUseCase(MediaClient& mediaClient, NetworkClient& networkClient);
        
        void prepare();
        void setOnStreamingStartCallback(function<void()> cb);
        void setOnStreamingStopCallback(function<void()> cb);
        void startStreaming();
        void stopStreaming();
};