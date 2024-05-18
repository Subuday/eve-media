#include <SpeakerAudioStreamingUseCase.h>

SpeakerAudioStreamingUseCase::SpeakerAudioStreamingUseCase(MediaClient& mediaClient, NetworkClient& networkClient) : mediaClient(mediaClient), networkClient(networkClient) {
    networkClient.setOnReceiveAudioCallback([this](vector<uint8_t> audio) {
        this->mediaClient.write(audio);
    });
}