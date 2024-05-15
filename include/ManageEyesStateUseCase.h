#pragma once

#include <EyesView.h>

class ManageEyesStateUseCase {
    private:
        EyesView& view;
    public:
        ManageEyesStateUseCase(EyesView& view);
        void openEyes();
        void closeEyes();
};