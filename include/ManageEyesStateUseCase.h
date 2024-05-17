#pragma once

#include <EyesView.h>

class ManageEyesStateUseCase {
    private:
        EyesView& view;
    public:
        ManageEyesStateUseCase(EyesView& view);
        void openEyes();
        void blink();
        void think();
        void speak();
        void trigger(int index = -1);
        void closeEyes();
};