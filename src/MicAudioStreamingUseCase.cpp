#include <MicAudioStreamingUseCase.h>

const string MicAudioStreamingUseCase::TAG = "[MicAudioStreamingUseCase] ";

MicAudioStreamingUseCase::MicAudioStreamingUseCase(MediaClient& mediaClient, NetworkClient& networkClient) : mediaClient(mediaClient), networkClient(networkClient) {}

void MicAudioStreamingUseCase::prepare() {
    mediaClient.recorder().setOnReadCallback([this](vector<int8_t> audio) {
        networkClient.sendAudio(audio);
        if (timer.active()) {
            timer.reset();
        }
    });
}

void MicAudioStreamingUseCase::setOnStreamingStartCallback(function<void()> cb) {
    onStreamingStartCallback = cb;
}

void MicAudioStreamingUseCase::setOnStreamingStopCallback(function<void()> cb) {
    onStreamingStopCallback = cb;
}

// TODO: Fix the issue when frames from previous steaming have not been sent yet, 
// and the new streaming is started with start packet.
void MicAudioStreamingUseCase::startStreaming() {
    timer.cancel();
    networkClient.sendStartRecording();
    mediaClient.recorder().startRecording();
    onStreamingStartCallback();
    cout << TAG << "Streaming started" << endl;
}

void MicAudioStreamingUseCase::stopStreaming() {
    mediaClient.recorder().stopRecording();
    timer.schedule([this] { networkClient.sendStopRecording(); }, 1000);
    isStreaming.store(false);
    onStreamingStopCallback();
    cout << TAG << "Streaming stopped" << endl;
}