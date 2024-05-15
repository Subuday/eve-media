#include <cassert> 
#include <stb_image.h>
#include <EyesView.h>

// Assert NONE state
void EyesView::updateState(EyesState state) {
    frameIndex = 0;
    this->state = state;
}

void EyesView::drawFrame(int width, int height, uint16_t* buffer, string path, int lastFrameIndex) {
    // TODO: Handle error
    string framePath = "../lib/eve-lcd-driver/res" + path + "/frame_" + to_string(frameIndex) + ".bmp";
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

    if (frameIndex == lastFrameIndex) {
        frameIndex = 0;
    } else {
        frameIndex += 1;
    }
}

void EyesView::draw(int width, int height, uint16_t* buffer) {
    switch (state) {
        case EyesState::NONE:
            return;
        case EyesState::OPENING:
            drawFrame(width, height, buffer, "/out", 90);
            break;
        case EyesState::CLOSING:
            drawFrame(width, height, buffer, "/thinking", 180);
            break;
        default:
            assert(false);
            // drawDefault(width, height, buffer);
            break;
    }
}