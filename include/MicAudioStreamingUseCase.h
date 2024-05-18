#pragma once

#include <atomic>
#include <MediaClient.h>
#include <NetworkClient.h>
#include <thread>

using namespace std;

class MicAudioStreamingUseCase {
    private:
        static const string TAG;

        MediaClient& mediaClient;
        NetworkClient& networkClient;

        thread workerThread;
        atomic<bool> isCancelled;
        atomic<bool> isStreaming;

        void work();
    public:
        MicAudioStreamingUseCase(MediaClient& mediaClient, NetworkClient& networkClient);
        void startStreaming();
        void stopStreaming();
};