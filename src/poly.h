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
        Vertex* vertices = new Vertex[14];
        Face* faces = new Face[24];
        Vertex* faceNormals = new Vertex[24];
        float xAngle = 0;
        float yAngle = 0;
        float zAngle = 0;

    public:
        void setup();
        void loadVertices();
        void loadFaces();
        void calculateNormals();
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

        // Overload operator* to apply the matrix transformation to a Vertex.
        Vertex operator*(const Vertex &v) const {
            Vertex result;
            result.x = r00 * v.x + r01 * v.y + r02 * v.z;
            result.y = r10 * v.x + r11 * v.y + r12 * v.z;
            result.z = r20 * v.x + r21 * v.y + r22 * v.z;
            return result;
        }

        // Optionally, if you need matrix multiplication, you can overload operator* for matrices.
        Matrix operator*(const Matrix &other) const {
            Matrix result;
            result.r00 = r00 * other.r00 + r01 * other.r10 + r02 * other.r20;
            result.r01 = r00 * other.r01 + r01 * other.r11 + r02 * other.r21;
            result.r02 = r00 * other.r02 + r01 * other.r12 + r02 * other.r22;

            result.r10 = r10 * other.r00 + r11 * other.r10 + r12 * other.r20;
            result.r11 = r10 * other.r01 + r11 * other.r11 + r12 * other.r21;
            result.r12 = r10 * other.r02 + r11 * other.r12 + r12 * other.r22;

            result.r20 = r20 * other.r00 + r21 * other.r10 + r22 * other.r20;
            result.r21 = r20 * other.r01 + r21 * other.r11 + r22 * other.r21;
            result.r22 = r20 * other.r02 + r21 * other.r12 + r22 * other.r22;
            return result;
        }        

    public:
        Matrix init(float xAngle, float yAngle, float zAngle);

};

class Render {

    public:
        Pixel proj3to2D(Vertex vertex, Screen screen);
        void projectAll2DPoints(Vertex *vertices, Pixel *projectedPoints, Screen screen, Matrix matrix);        
        void drawObject(Tetrakis tetrakis, uint32_t *pixels, Screen screen);
        void drawFace(Face face, Pixel *projectedPoints, Vertex faceNormal, Screen screen, uint32_t *pixels,  Matrix matrix);
        void drawAllFaces(Face *face, Pixel *projectedPoints, Vertex *faceNormal, Screen screen, uint32_t *pixels, Matrix matrix);
};
