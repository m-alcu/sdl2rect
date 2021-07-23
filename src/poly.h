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


const uint8_t desertBase[40] = { 
    15,15,16,16,17,19,21,23,26,29,31,31,31,35,39,42,45,43,60,57,
    55,51,47,45,44,42,39,37,34,31,29,25,21,19,16,16,16,15,15,15  
};

class Desert {

    public:
        uint16_t seed1;
        uint16_t seed2;

    public:
        void calcPalette(uint32_t *palette);

    public:
        void draw(uint32_t *pixels, Screen screen, uint32_t *palette, uint8_t *greys);

};

class Rectangle {

    public:
        Pixel start;
        Pixel end;
        uint32_t color;

    public:
        void draw(uint32_t *pixels, Screen screen);

    public:
        void randomDraw(uint32_t *pixels, Screen screen);

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

    public:
        void draw(uint32_t *pixels, Screen screen);

    public:
        void randomDraw(uint32_t *pixels, Screen screen);

    private:
        void drawTriSector(uint16_t top, uint16_t bottom, int32_t *leftSide, int32_t *rightSide, uint32_t *pixels, Screen screen, int32_t leftEdge, int32_t rightEdge);

    private:
        void orderPixels(Pixel *p1, Pixel *p2, Pixel *p3);
    
    private:
        int32_t calculateEdge(Pixel p1, Pixel p2);

    private:
        void calculateEdges(Pixel p1, Pixel p2, Pixel p3);

    public:
        bool visible();

};


class Loader {

    public:
        void loadVertices(Vertex *vertices);

    public:
        void loadNormals(Vertex *normals);

    public:
        void loadFaces(Face *faces);

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

    public:
        Vertex rotate(Vertex vertex);

};

class Render {

    public:
        Pixel proj3to2D(Vertex vertex, Screen screen);

    public:
        void rotateAllVertices(Vertex *vertices, Vertex *rotatedVertices, Matrix matrix);

    public:
        void projectAll2DPoints(Vertex *vertices, Pixel *projectedPoints, Screen screen);        

    public:
        void drawObject(Face *faces, Vertex *vertices, Vertex *rotatedVertices, Vertex *faceNormals, Pixel *projectedPoints, uint32_t *pixels, Screen screen, double xAngle, double yAngle, double zAngle);

    public:
        void drawFace(Face face, Pixel *projectedPoints, Vertex faceNormal, Screen screen, uint32_t *pixels,  Matrix matrix);

    public:
        void drawAllFaces(Face *face, Pixel *projectedPoints, Vertex *faceNormal, Screen screen, uint32_t *pixels, Matrix matrix);

};
