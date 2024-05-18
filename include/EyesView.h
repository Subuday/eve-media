#pragma once

#include <View.hpp>
#include <EyesState.h>
#include <Timer.h>
#include <string>
#include <functional>

using namespace std;

class EyesView: public View {
    private:
        function<void()> onAnimationStartCallback;
        function<void()> onAnimationEndCallback;

        int frameChannels = 3;

        EyesState state = EyesState::NONE;

        int frameIndex = 0;

        int animationFrames(EyesState state);
        void drawFrame(int width, int height, uint16_t* buffer, string animation);
        void drawFrame(EyesState state, int width, int height, uint16_t* buffer);
    public:
        void draw(int width, int height, uint16_t* buffer) override;

        void setOnAnimaitonStartCallback(function<void()> cb);
        void setOnAnimationEndCallback(function<void()> cb);

        void updateState(EyesState state);
};