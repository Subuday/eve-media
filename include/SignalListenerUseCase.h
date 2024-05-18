#pragma once

#include <functional>
#include <vector>

using namespace std;

class SignalListenerUseCase {
    private:
        static vector<function<void()>> cbs;
        static void onSignal(int signal);
    public:
        SignalListenerUseCase();
        static void callbacks(vector<function<void()>> cbs);
};