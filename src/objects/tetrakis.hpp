#pragma once


#include "solid.hpp"

class Tetrakis : public Solid {
    public:
        // Constructor that takes the number of vertices and faces as parameters. https://stackoverflow.com/questions/16119762/destructor-of-an-object-causes-crashing
        Tetrakis()
    {
    }

    protected:   
        void loadVertices() override;
        void loadFaces() override;
    };


