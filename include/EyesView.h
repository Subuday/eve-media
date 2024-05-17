#pragma once

#include <View.hpp>
#include <EyesState.h>
#include <string>

using namespace std;

class EyesView: public View {
    private:
        int frameChannels = 3;

        EyesState state = EyesState::NONE;

        int frameIndex = 0;

        void drawFrame(int width, int height, uint16_t* buffer, string animation, int frames);
    public:
        void updateState(EyesState state);
        void draw(int width, int height, uint16_t* buffer) override;
};