#include <EyesState.h>
#include <ManageEyesStateUseCase.h>

ManageEyesStateUseCase::ManageEyesStateUseCase(EyesView& view) : view(view) {}

void ManageEyesStateUseCase::openEyes() {
    view.updateState(EyesState::OPENING);
}

void ManageEyesStateUseCase::blink() {
    view.updateState(EyesState::BLINKING);
}

void ManageEyesStateUseCase::think() {
    view.updateState(EyesState::THINKING);
}

void ManageEyesStateUseCase::speak() {
    view.updateState(EyesState::SPEAKING);
}

void ManageEyesStateUseCase::trigger(int index) {
    switch (index) {
        case 0:
            view.updateState(EyesState::SMILING);
            break;
        case 1:
            view.updateState(EyesState::DVD);
            break;
        case 2:
            view.updateState(EyesState::JUMPING);
            break;
        default:
            break;
    }
}

void ManageEyesStateUseCase::closeEyes() {
    view.updateState(EyesState::CLOSING);
}