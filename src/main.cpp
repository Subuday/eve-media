#include <App.h>

int main(int argc, char *argv[]) {
    App& app = App::instance();
    app.start();
    return 0;
}