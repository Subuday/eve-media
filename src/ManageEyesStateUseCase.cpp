#include <EyesState.h>
#include <ManageEyesStateUseCase.h>

ManageEyesStateUseCase::ManageEyesStateUseCase(EyesView& view) : view(view) {}

void ManageEyesStateUseCase::openEyes() {
    view.updateState(EyesState::OPENING);
}

void ManageEyesStateUseCase::closeEyes() {
    view.updateState(EyesState::CLOSING);
}