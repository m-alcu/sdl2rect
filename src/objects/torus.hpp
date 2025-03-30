#pragma once


#include "solid.hpp"

class Torus : public Solid {
    public:
        // Constructor that takes the number of vertices and faces as parameters. https://stackoverflow.com/questions/16119762/destructor-of-an-object-causes-crashing
        Torus()
    {
    }

    public:
       void setup(int uSteps, int vSteps, float R, float r);

    private:
        void loadVertices( int uSteps, int vSteps, float R, float r );
        void loadFaces(int uSteps, int vSteps);

    protected:   
        void loadVertices() override;
        void loadFaces() override;
    };


