#ifndef TETRAKIS_HPP
#define TETRAKIS_HPP

#include "space3d.hpp"

class Tetrakis {
    public:
        Vertex* vertices;
        Face* faces;
        Vertex* faceNormals;
        int numVertices;
        int numFaces;
        float xAngle;
        float yAngle;
        float zAngle;
    
    public:
        // Constructor that takes the number of vertices and faces as parameters. https://stackoverflow.com/questions/16119762/destructor-of-an-object-causes-crashing
        Tetrakis(int verticesCount, int facesCount)
            : numVertices(verticesCount), numFaces(facesCount),
              vertices(new Vertex[verticesCount]),
              faces(new Face[facesCount]),
              faceNormals(new Vertex[facesCount]) // Assuming one normal per face
        {
        }
    
        // Destructor to deallocate memory
        ~Tetrakis() {
            delete[] vertices;
            delete[] faces;
            delete[] faceNormals;
        }
    
        void setup();
        // Other member functions
    private:   
        void loadVertices();
        void loadFaces();
        void calculateNormals();
    };

#endif
