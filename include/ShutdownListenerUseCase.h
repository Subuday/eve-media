#pragma once

#include <functional> 

using namespace std;

class ShutdownListenerUseCase {
    private:
        static function<void()> cb;
        static void onSignal(int signal);
    public:
        ShutdownListenerUseCase();
        void callback(function<void()> cb);
};