#pragma once

#include <View.hpp>
#include <EyesState.h>

class EyesView: public View {
    private:
        int frameChannels = 3;

        EyesState state = EyesState::NONE;

        int frameIndex = 0;

        void drawOpeningAnimationFrame(int width, int height, uint16_t* buffer);
    public:
        void updateState(EyesState state);
        void draw(int width, int height, uint16_t* buffer) override;
};