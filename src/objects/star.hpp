#pragma once


#include "solid.hpp"

class Star : public Solid {
    public:
        Star(int verticesCount, int facesCount)
        : Solid(verticesCount, facesCount)
    {
    }

    protected:   
        void loadVertices() override;
        void loadFaces() override;
    };

