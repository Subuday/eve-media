#include <ButtonClient.h>
#include <bcm2835.h>
#include <chrono>
#include <thread>

ButtonClient::ButtonClient() {
    // cout << "ButtonClient::ButtonClient()" << endl;
}

void ButtonClient::prepare() {
    if (!bcm2835_init()) {
        cout << "bcm2835_init() failed. Are you running as root?" << endl;
        return;
    }
    cout << "ButtonClient::prepare()" << endl;
    setupRecordingButton();
}

void ButtonClient::setOnButtonPressCallback(function<void(bool)> cb) {
    onButtonPressCallback = cb;
}

void ButtonClient::release() {
    bcm2835_close();
    cout << "ButtonClient::release()" << endl;
}

void ButtonClient::setupRecordingButton() {
    bcm2835_gpio_fsel(RPI_BPLUS_GPIO_J8_37, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(RPI_BPLUS_GPIO_J8_37, BCM2835_GPIO_PUD_UP);
    thread([this]() {
        bool isPressed = false;
        while (true) {
            if (bcm2835_gpio_lev(RPI_BPLUS_GPIO_J8_37) == LOW) {
                if (!isPressed) {
                    isPressed = true;
                    onButtonPressCallback(isPressed);
                }
            } else {
                if (isPressed) {
                    isPressed = false;
                    onButtonPressCallback(isPressed);
                }
            }
            this_thread::sleep_for(chrono::seconds(1));
        }
    }).detach();

}

ButtonClient::~ButtonClient() {
    // cout << "ButtonClient::~ButtonClient()" << endl;
}