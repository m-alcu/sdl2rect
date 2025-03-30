#pragma once


#include "solid.hpp"

class Star : public Solid {
    public:
        Star()
    {
    }

    protected:   
        void loadVertices() override;
        void loadFaces() override;
    };

