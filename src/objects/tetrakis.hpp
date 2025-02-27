#ifndef TETRAKIS_HPP
#define TETRAKIS_HPP

#include "solid.hpp"

class Tetrakis : public Solid {
    public:
        // Constructor that takes the number of vertices and faces as parameters. https://stackoverflow.com/questions/16119762/destructor-of-an-object-causes-crashing
        Tetrakis(int verticesCount, int facesCount)
        : Solid(verticesCount, facesCount)
    {
    }

    protected:   
        void loadVertices() override;
        void loadFaces() override;
    };

#endif
