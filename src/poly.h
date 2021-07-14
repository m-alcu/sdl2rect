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
        uint32_t edge12; // 16.16
        uint32_t edge23; // 16.16
        uint32_t edge13; // 16.16

    public:
        void draw(uint32_t *pixels, Screen screen);

    public:
        void randomDraw(uint32_t *pixels, Screen screen);

    private:
        void drawSector(uint16_t top, uint16_t bottom, int32_t *leftSide, int32_t *rightSide, uint32_t *pixels, Screen screen, int32_t leftEdge, int32_t rightEdge);

    private:
        void orderPixels(Pixel *p1, Pixel *p2, Pixel *p3);
    
    private:
        int32_t calculateEdge(Pixel p1, Pixel p2);

    private:
        void calculateEdges(Pixel p1, Pixel p2, Pixel p3);



};