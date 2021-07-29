#include <iostream>

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
    double x;
    double y;
    double z;
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
        void calcPalette(uint32_t *palette);
        void draw(uint32_t *pixels, Screen screen, uint32_t *palette, uint8_t *greys);

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
        void swapNum( uint16_t *x, uint16_t *y);
        void swapPixel(Pixel *p1, Pixel *p2);

};


class Loader {

    public:
        void loadVertices(Vertex *vertices);
        void loadFaces(Face *faces);
        void calculateNormals(Face *faces, Vertex *normals, Vertex *vertices);
};


class Matrix {

    public:
        double r00;
        double r01;
        double r02;

        double r10;
        double r11;
        double r12;

        double r20;
        double r21;
        double r22;

    public:
        void init(double xAngle, double yAngle, double zAngle);
        Vertex rotate(Vertex vertex);

};

class Render {

    public:
        Pixel proj3to2D(Vertex vertex, Screen screen);
        void rotateAllVertices(Vertex *vertices, Vertex *rotatedVertices, Matrix matrix);
        void projectAll2DPoints(Vertex *vertices, Pixel *projectedPoints, Screen screen);        
        void drawObject(Face *faces, Vertex *vertices, Vertex *faceNormals, uint32_t *pixels, Screen screen, double xAngle, double yAngle, double zAngle);
        void drawFace(Face face, Pixel *projectedPoints, Vertex faceNormal, Screen screen, uint32_t *pixels,  Matrix matrix);
        void drawAllFaces(Face *face, Pixel *projectedPoints, Vertex *faceNormal, Screen screen, uint32_t *pixels, Matrix matrix);
};
