#include <SpeakerAudioStreamingUseCase.h>

SpeakerAudioStreamingUseCase::SpeakerAudioStreamingUseCase(MediaClient& mediaClient, NetworkClient& networkClient) : mediaClient(mediaClient), networkClient(networkClient) {
    networkClient.setOnReceiveAudioCallback([this](vector<uint8_t> audio) {
        this->mediaClient.player().write(audio);
    });
}

void SpeakerAudioStreamingUseCase::setOnStreamingStartCallback(function<void()> cb) {
    onStreamingStartCallback = cb;
}

void SpeakerAudioStreamingUseCase::setOnStreamingStopCallback(function<void()> cb) {
    onStreamingStopCallback = cb;
}

void SpeakerAudioStreamingUseCase::interrupt() {
    mediaClient.player().interrupt();
}