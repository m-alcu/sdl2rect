#include <iostream>

typedef struct Screen
{
    uint16_t high;
    uint16_t width;
} Screen;

typedef struct Vertex
{
    uint16_t x;
    uint16_t y;
} Vertex;

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


class Rectangle {

    public:
        Vertex start;
        Vertex end;
        uint32_t color;

    public:
        void draw(uint32_t *pixels, Screen screen);

    public:
        void randomDraw(uint32_t *pixels, Screen screen);

};

class Triangle {

    public:
        Vertex p1;
        Vertex p2;
        Vertex p3;
        uint32_t color;
        uint32_t edge12; // 16.16
        uint32_t edge23; // 16.16
        uint32_t edge13; // 16.16

    public:
        void randomDraw(uint32_t *pixels, Screen screen);

    private:
        int32_t getEdge(Vertex p1, Vertex p2);

    private:
        void drawSector(uint16_t top, uint16_t bottom, int32_t *leftSide, int32_t *rightSide, uint32_t *pixels, Screen screen, int32_t leftEdge, int32_t rightEdge);
};