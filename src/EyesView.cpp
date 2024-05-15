#include <cassert> 
#include <string>
#include <stb_image.h>
#include <EyesView.h>

using namespace std;

// Assert NONE state
void EyesView::updateState(EyesState state) {
    this->state = state;
}

void EyesView::drawOpeningAnimationFrame(int width, int height, uint16_t* buffer) {
    // TODO: Handle error
    string path = "../lib/eve-lcd-driver/res/out/frame_" + to_string(frameIndex) + ".bmp";
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &frameChannels, 0);

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

void EyesView::draw(int width, int height, uint16_t* buffer) {
    switch (state) {
        case EyesState::NONE:
            return;
        case EyesState::OPENING:
            drawOpeningAnimationFrame(width, height, buffer);
            break;
        case EyesState::CLOSING:
            // drawClosing(width, height, buffer);
            break;
        default:
            assert(false);
            // drawDefault(width, height, buffer);
            break;
    }
}