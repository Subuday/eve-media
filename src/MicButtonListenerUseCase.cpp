#include <csignal>
#include <MicButtonListenerUseCase.h>
#include <stdio.h>

function<void(bool)> MicButtonListenerUseCase::cb;

MicButtonListenerUseCase::MicButtonListenerUseCase() {
    signal(60, MicButtonListenerUseCase::onSignal);
    signal(61, MicButtonListenerUseCase::onSignal);
}

void MicButtonListenerUseCase::onSignal(int signal) {
    if (signal == 60) {
        MicButtonListenerUseCase::cb(true);
    } else {
        MicButtonListenerUseCase::cb(false);
    }
}

void MicButtonListenerUseCase::callback(function<void(bool)> cb) {
    MicButtonListenerUseCase::cb = cb;
}