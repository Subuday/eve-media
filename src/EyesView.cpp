#include <stb_image.h>
#include <EyesView.h>

void EyesView::draw(int width, int height, uint16_t* buffer) {
    // TODO: Handle error
    unsigned char* data = stbi_load("../lib/eve-lcd-driver/res/smiling/frame_10.bmp", &width, &height, &frameChannels, 0);

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