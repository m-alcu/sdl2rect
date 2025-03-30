#pragma once


#include "solid.hpp"

class ObjLoader : public Solid {
    public:
        ObjLoader()
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

