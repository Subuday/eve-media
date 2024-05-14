#pragma once

#include <View.hpp>

class EyesView: public View {
    private:
        int frameChannels = 3;
    public:
        void draw(int width, int height, uint16_t* buffer) override;
};