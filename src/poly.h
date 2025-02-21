#include <iostream>
#include <cstdint>

typedef struct Screen
{
    uint16_t high;
    uint16_t width;
} Screen;

typedef struct Pixel
{
    uint16_t x;
    uint16_t y;
} Pixel;

typedef union
{
    struct
    {
        unsigned char blue;
        unsigned char green;
        unsigned char red;
        unsigned char alpha;
    } rgba;

    uint32_t long_value;
} RGBValue;

typedef struct Vertex
{
    float x;
    float y;
    float z;
} Vertex;

typedef struct Face
{
    int16_t vertex1;
    int16_t vertex2;
    int16_t vertex3;
    int32_t color;
} Face;


class Desert {

    public:
        uint16_t seed1;
        uint16_t seed2;

    public:
        void draw(uint32_t *pixels, Screen screen);

    private:
        void calcPalette(uint32_t *palette);
};

class Triangle {

    public:
        Pixel p1;
        Pixel p2;
        Pixel p3;
        uint32_t color;  // RGBA
        int32_t edge12; // 16.16
        int32_t edge23; // 16.16
        int32_t edge13; // 16.16
        Screen screen;
        uint32_t *pixels;
        Triangle(uint32_t *pixelsAux, Screen screenAux) {
            screen = screenAux;
            pixels = pixelsAux;
        }

    public:
        void draw();
        bool visible();

    private:
        void drawTriSector(uint16_t top, uint16_t bottom, int32_t *leftSide, int32_t *rightSide, uint32_t *pixels, Screen screen, int32_t leftEdge, int32_t rightEdge);
        void orderPixels(Pixel *p1, Pixel *p2, Pixel *p3);
        int32_t calculateEdge(Pixel p1, Pixel p2);
        void calculateEdges(Pixel p1, Pixel p2, Pixel p3);
        void swapPixel(Pixel *p1, Pixel *p2);

};


class Tetrakis {

    public:
        void loadVertices(Vertex *vertices);
        void loadFaces(Face *faces);
        void calculateNormals(Face *faces, Vertex *normals, Vertex *vertices);
};


class Matrix {

    public:
        float r00;
        float r01;
        float r02;

        float r10;
        float r11;
        float r12;

        float r20;
        float r21;
        float r22;

    public:
        void init(float xAngle, float yAngle, float zAngle);
        Vertex rotate(Vertex vertex);

};

class Render {

    public:
        Pixel proj3to2D(Vertex vertex, Screen screen);
        void rotateAllVertices(Vertex *vertices, Vertex *rotatedVertices, Matrix matrix);
        void projectAll2DPoints(Vertex *vertices, Pixel *projectedPoints, Screen screen);        
        void drawObject(Face *faces, Vertex *vertices, Vertex *faceNormals, uint32_t *pixels, Screen screen, float xAngle, float yAngle, float zAngle);
        void drawFace(Face face, Pixel *projectedPoints, Vertex faceNormal, Screen screen, uint32_t *pixels,  Matrix matrix);
        void drawAllFaces(Face *face, Pixel *projectedPoints, Vertex *faceNormal, Screen screen, uint32_t *pixels, Matrix matrix);
};
