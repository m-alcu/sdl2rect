#pragma once


#include "solid.hpp"

class AscLoader : public Solid {
    public:
        AscLoader()
    {
    }

    public:
        void setup(const std::string& filename);

    private:
        void loadVertices(const std::string& filename);

    protected:   
        void loadVertices() override;
        void loadFaces() override;

    };

