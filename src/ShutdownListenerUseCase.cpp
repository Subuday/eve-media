#include <csignal>
#include <ShutdownListenerUseCase.h>

function<void()> ShutdownListenerUseCase::cb = nullptr;

ShutdownListenerUseCase::ShutdownListenerUseCase() {
    signal(SIGINT, ShutdownListenerUseCase::onSignal);
}

void ShutdownListenerUseCase::onSignal(int signal) {
    ShutdownListenerUseCase::cb();
}

void ShutdownListenerUseCase::callback(function<void()> cb) {
    ShutdownListenerUseCase::cb = cb;
}
