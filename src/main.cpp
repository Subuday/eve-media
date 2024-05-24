
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <App.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>

int main(int argc, char *argv[]) {
    App& app = App::instance();
    app.run();
    return 0;
}
