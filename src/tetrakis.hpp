#ifndef TETRAKIS_HPP
#define TETRAKIS_HPP

#include "space3d.hpp"
#include "solid.hpp"

class Tetrakis : public Solid {
    public:
        // Constructor that takes the number of vertices and faces as parameters. https://stackoverflow.com/questions/16119762/destructor-of-an-object-causes-crashing
        Tetrakis(int verticesCount, int facesCount)
        : Solid(verticesCount, facesCount)
    {
    }
    
        // Optionally, override setup if additional Tetrakis-specific setup is needed.
        void setup() override {
            Solid::setup(); // calls loadVertices, loadFaces, calculateNormals
            // Add Tetrakis-specific initialization here if necessary.
        }
        // Other member functions
    protected:   
        void loadVertices() override;
        void loadFaces() override;
        void calculateNormals() override;
    };

#endif
