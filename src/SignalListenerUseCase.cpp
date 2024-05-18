#include <csignal>
#include <SignalListenerUseCase.h>
#include <stdio.h>

vector<function<void()>> SignalListenerUseCase::cbs;

SignalListenerUseCase::SignalListenerUseCase() {
    for (int i = SIGRTMIN; i <= SIGRTMAX; ++i) {
        signal(i, SignalListenerUseCase::onSignal);
    }
}

void SignalListenerUseCase::onSignal(int signal) {
    int index = signal - SIGRTMIN;
    if (cbs.size() > index) {
        cbs[index]();
    }
}

void SignalListenerUseCase::callbacks(vector<function<void()>> cbs) {
    SignalListenerUseCase::cbs = cbs;
}