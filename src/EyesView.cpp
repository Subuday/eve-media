#include <cassert> 
#include <stb_image.h>
#include <EyesView.h>

void EyesView::setOnAnimaitonStartCallback(function<void()> cb) {
    onAnimationStartCallback = cb;
}

void EyesView::setOnAnimationEndCallback(function<void()> cb) {
    onAnimationEndCallback = cb;
}

int EyesView::animationFrames(EyesState state) {
    switch (state) {
        case EyesState::OPENING:
            return 362;
        case EyesState::BLINKING:
            return 363;
        case EyesState::THINKING:
            return 398;
        case EyesState::SPEAKING:
            return 333;
        case EyesState::SMILING:
            return 333;
        case EyesState::DVD:
            return 363;
        case EyesState::JUMPING:
            return 363;
        case EyesState::CLOSING:
            return 363;
        default:
            // TODO: assert
            return 0;
    }
}

void EyesView::drawFrame(int width, int height, uint16_t* buffer, string animation) {
    // TODO: Handle error
    string framePath = "../lib/eve-lcd-driver/res/" + animation + "/frame_" + to_string(frameIndex) + ".png";
    unsigned char* data = stbi_load(framePath.c_str(), &width, &height, &frameChannels, 0);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            unsigned char* pixel = data + frameChannels * (y * width + x);
            unsigned char red = pixel[0];
            unsigned char green = pixel[1];
            unsigned char blue = pixel[2];

            uint8_t r = (red >> 3) & 0x1F;   // Reduce to 5 bits
            uint8_t g = (green >> 2) & 0x3F; // Reduce to 6 bits
            uint8_t b = (blue >> 3) & 0x1F;  // Reduce to 5 bits

            uint16_t rgb16 = (r << 11) | (g << 5) | b;
            buffer[y * width + x] = rgb16;
        }
    }

    stbi_image_free(data);
}

void EyesView::drawFrame(EyesState statem, int width, int height, uint16_t* buffer) {
    switch (state) {
        case EyesState::NONE:
            return;
        case EyesState::OPENING:
            drawFrame(width, height, buffer, "opening");
            break;
        case EyesState::BLINKING:
            drawFrame(width, height, buffer, "blinking");
            break;
        case EyesState::THINKING:
            drawFrame(width, height, buffer, "thinking");
            break;
        case EyesState::SPEAKING:
            drawFrame(width, height, buffer, "speaking");
            break;
        case EyesState::SMILING:
            drawFrame(width, height, buffer, "smiling");
            break;
        case EyesState::DVD:
            drawFrame(width, height, buffer, "dvd");
            break;
        case EyesState::JUMPING:
            drawFrame(width, height, buffer, "jumping");
            break;
        case EyesState::CLOSING:
            drawFrame(width, height, buffer, "closing");
            break;
        default:
            assert(false);
            // drawDefault(width, height, buffer);
            return;
    }
}

void EyesView::draw(int width, int height, uint16_t* buffer) {
    if (frameIndex == 0 && onAnimationStartCallback) {
        onAnimationStartCallback();
    }
    
    drawFrame(state, width, height, buffer);

    if (frameIndex == animationFrames(state) - 1) {
        onAnimationEndCallback();
    } else {
        frameIndex += 1;
    }
}

// Assert NONE state
void EyesView::updateState(EyesState state) {
    frameIndex = 0;
    this->state = state;
}