#include <cassert> 
#include <stb_image.h>
#include <EyesView.h>

// Assert NONE state
void EyesView::updateState(EyesState state) {
    frameIndex = 0;
    this->state = state;
}

void EyesView::drawFrame(int width, int height, uint16_t* buffer, string animation, int frames) {
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

    if (frameIndex == frames - 1) {
        // frameIndex = 0;
    } else {
        frameIndex += 1;
    }
}

void EyesView::draw(int width, int height, uint16_t* buffer) {
    switch (state) {
        case EyesState::NONE:
            return;
        case EyesState::OPENING:
            drawFrame(width, height, buffer, "opening", 362);
            break;
        case EyesState::BLINKING:
            drawFrame(width, height, buffer, "blinking", 363);
            break;
        case EyesState::THINKING:
            drawFrame(width, height, buffer, "thinking", 398);
            break;
        case EyesState::SPEAKING:
            drawFrame(width, height, buffer, "speaking", 333);
            break;
        case EyesState::SMILING:
            drawFrame(width, height, buffer, "smiling", 333);
            break;
        case EyesState::DVD:
            drawFrame(width, height, buffer, "dvd", 363);
            break;
        case EyesState::JUMPING:
            drawFrame(width, height, buffer, "jumping", 363);
            break;
        case EyesState::CLOSING:
            drawFrame(width, height, buffer, "closing", 363);
            break;
        default:
            assert(false);
            // drawDefault(width, height, buffer);
            break;
    }
}