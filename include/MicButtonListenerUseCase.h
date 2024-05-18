#pragma once

#include <functional>
#include <vector>

using namespace std;

class MicButtonListenerUseCase {
    private:
        static function<void(bool)> cb;
        static void onSignal(int signal);
    public: 
        MicButtonListenerUseCase();
        static void callback(function<void(bool)> cb);
};