#include <MicAudioStreamingUseCase.h>

const string MicAudioStreamingUseCase::TAG = "[MicAudioStreamingUseCase] ";

MicAudioStreamingUseCase::MicAudioStreamingUseCase(MediaClient& mediaClient, NetworkClient& networkClient) : mediaClient(mediaClient), networkClient(networkClient), workerThread(&MicAudioStreamingUseCase::work, this) {

}

void MicAudioStreamingUseCase::work() {
    while(!isCancelled.load()) {
        // const auto data = mediaClient.read();
        // if (!data.empty() && isStreaming.load()) {
        //     networkClient.sendAudio(data);
        // }
    }
}

void MicAudioStreamingUseCase::startStreaming() {
    isStreaming.store(true);
    cout << TAG << "Streaming started" << endl;
}

void MicAudioStreamingUseCase::stopStreaming() {
    isStreaming.store(false);
    cout << TAG << "Streaming stopped" << endl;
}