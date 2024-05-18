#pragma once

#include <EyesView.h>
#include <EyesState.h>
#include <mutex>

using namespace std;

class ManageEyesStateUseCase {
    private:
        EyesView& view;
        
        recursive_mutex mtx;
        EyesState currentState;
        EyesState pendingState;
        int consecutiveBlinkCount = 0;

        void updateState();
        void setPendingState(EyesState state);
        
        EyesState randomTriggeringState();
        EyesState nextState(EyesState currentState, EyesState pendingState);
    public:
        ManageEyesStateUseCase(EyesView& view);
        void openEyes();
        void blink();
        void think();
        void speak();
        void trigger(int index);
        void closeEyes();
};