#include <EyesState.h>
#include <ManageEyesStateUseCase.h>

ManageEyesStateUseCase::ManageEyesStateUseCase(EyesView& view) : view(view), currentState(EyesState::NONE), pendingState(EyesState::NONE) {
    view.setOnAnimationEndCallback([this]() {
        updateState();
    });
}

void ManageEyesStateUseCase::updateState() {
    EyesState nextState;
    {
        lock_guard<recursive_mutex> lock(mtx);

        nextState = this->nextState(currentState, pendingState);
        EyesState nextPendingState = EyesState::NONE;

        if (nextState == EyesState::BLINKING) {
            consecutiveBlinkCount++;
            if (consecutiveBlinkCount >= 2) {
                nextPendingState = randomTriggeringState();
                consecutiveBlinkCount = 0;
            }
        } else {
            consecutiveBlinkCount = 0;
        }
        
        currentState = nextState;
        pendingState = nextPendingState;
    }
    
    // TODO: Post on main thread
    view.updateState(nextState);
}

void ManageEyesStateUseCase::setPendingState(EyesState state) {
    lock_guard<recursive_mutex> lock(mtx);
    pendingState = state;
}

EyesState ManageEyesStateUseCase::randomTriggeringState() {
    return static_cast<EyesState>(rand() % 3 + 5);
}

EyesState ManageEyesStateUseCase::nextState(EyesState currentState, EyesState pendingState) {
    if (pendingState == EyesState::SMILING ||
        pendingState == EyesState::DVD ||
        pendingState == EyesState::JUMPING) {
        if (currentState == EyesState::BLINKING) {
            return pendingState;
        } else {
            pendingState == EyesState::NONE; // Ignore triggering state
        }
    }

    if (pendingState == EyesState::NONE) {
        if (currentState == EyesState::SPEAKING) {
            return EyesState::SPEAKING;
        } else if (currentState == EyesState::THINKING) {
            return EyesState::THINKING;
        } else if (currentState == EyesState::CLOSING) {
            return EyesState::NONE;
        } else {
            return EyesState::BLINKING;
        }
    }

    return pendingState;
}

void ManageEyesStateUseCase::openEyes() {
    setPendingState(EyesState::OPENING);
    updateState();
}

void ManageEyesStateUseCase::blink() {
    setPendingState(EyesState::BLINKING);
}

void ManageEyesStateUseCase::think() {
    setPendingState(EyesState::THINKING);
    updateState();
}

void ManageEyesStateUseCase::speak() {
    setPendingState(EyesState::SPEAKING);
    updateState();
}

void ManageEyesStateUseCase::trigger(int index) {
    switch (index) {
        case 0:
            setPendingState(EyesState::SMILING);
            break;
        case 1:
            setPendingState(EyesState::DVD);
            break;
        case 2:
            setPendingState(EyesState::JUMPING);
            break;
        default:
            break;
    }
}

void ManageEyesStateUseCase::closeEyes() {
    setPendingState(EyesState::CLOSING);
    updateState();
}