#ifndef TEST_HPP
#define TEST_HPP

#include "solid.hpp"

class Test : public Solid {
    public:
        // Constructor that takes the number of vertices and faces as parameters. https://stackoverflow.com/questions/16119762/destructor-of-an-object-causes-crashing
        Test(int verticesCount, int facesCount)
        : Solid(verticesCount, facesCount)
    {
    }

    protected:   
        void loadVertices() override;
        void loadFaces() override;
    };

#endif
