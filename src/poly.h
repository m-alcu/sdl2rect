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
        uint32_t edge12; // 16.16
        uint32_t edge23; // 16.16
        uint32_t edge13; // 16.16

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



};