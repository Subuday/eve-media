#include <iostream>
#include <functional>

using namespace std;

class ButtonClient {
    private:
        function<void(bool)> onButtonPressCallback;
        void setupRecordingButton();
    public:
        ButtonClient();
        ~ButtonClient();

        void prepare();
        void setOnButtonPressCallback(function<void(bool)> cb);
        void release();
};