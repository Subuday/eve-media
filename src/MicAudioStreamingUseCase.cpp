#include <MicAudioStreamingUseCase.h>

const string MicAudioStreamingUseCase::TAG = "[MicAudioStreamingUseCase] ";

MicAudioStreamingUseCase::MicAudioStreamingUseCase(MediaClient& mediaClient, NetworkClient& networkClient) : mediaClient(mediaClient), networkClient(networkClient) {}

void MicAudioStreamingUseCase::prepare() {
    mediaClient.recorder().setOnReadCallback([this](vector<int8_t> audio) {
        networkClient.sendAudio(audio);
    });
}

void MicAudioStreamingUseCase::setOnStreamingStartCallback(function<void()> cb) {
    onStreamingStartCallback = cb;
}

void MicAudioStreamingUseCase::setOnStreamingStopCallback(function<void()> cb) {
    onStreamingStopCallback = cb;
}

void MicAudioStreamingUseCase::startStreaming() {
    mediaClient.recorder().startRecording();
    onStreamingStartCallback();
    cout << TAG << "Streaming started" << endl;
}

void MicAudioStreamingUseCase::stopStreaming() {
    mediaClient.recorder().stopRecording();
    onStreamingStopCallback();
    cout << TAG << "Streaming stopped" << endl;
}