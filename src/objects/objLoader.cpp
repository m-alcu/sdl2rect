#include <iostream>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <regex>
#include "../rasterizer.hpp"

void ObjLoader::setup(const std::string& filename) {
    loadVertices(filename);
    loadFaces();
    calculateNormals();
    calculateVertexNormals();
}

void ObjLoader::loadVertices(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open file.\n";
        return;
    }

    std::string line;
    bool readingVertices = false;
    bool readingFaces = false;

    std::vector<slib::vec3> vertices;
    std::vector<Face> faces;

    MaterialProperties properties = getMaterialProperties(MaterialType::Metal);

    while (std::getline(file, line)) {
        // Remove leading/trailing spaces
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        // Skip empty lines
        if (line.empty())
            continue;


        if (line.find("v") != std::string::npos) {
            // Example line: v -43.592037 7.219297 -21.717901
            slib::vec3 vertex;
            std::regex vertexRegex(R"(^v\s+([-+\.\dEe]+)\s+([-+\.\dEe]+)\s+([-+\.\dEe]+))");
            std::smatch match;

            if (std::regex_search(line, match, vertexRegex)) {
                vertex.x = std::stof(match[1]);
                vertex.y = std::stof(match[2]);
                vertex.z = std::stof(match[3]);

                vertices.push_back(vertex);
            }
        }

        if (line.find("f") != std::string::npos) {
            // Example line: f 791 763 645
            Face face;
            std::regex faceRegex(R"(^f\s+(\d+)\s+(\d+)\s+(\d+)$)");
            std::smatch match;

            if (std::regex_search(line, match, faceRegex)) {
                face.vertex1 = std::stoi(match[3])-1;
                face.vertex2 = std::stoi(match[2])-1;
                face.vertex3 = std::stoi(match[1])-1;
                face.material.Ka = { getColorFromMaterial(properties.k_a * 0x00), getColorFromMaterial(properties.k_a * 0x58), getColorFromMaterial(properties.k_a * 0xfc) };
                face.material.Kd = { getColorFromMaterial(properties.k_d * 0x00), getColorFromMaterial(properties.k_d * 0x58), getColorFromMaterial(properties.k_d * 0xfc) };
                face.material.Ks = { getColorFromMaterial(properties.k_s * 0x00), getColorFromMaterial(properties.k_s * 0x58), getColorFromMaterial(properties.k_s * 0xfc) };
                face.material.Ns = properties.shininess;

                faces.push_back(face);
            }
        }
    }

    file.close();

    // Calculate total number of vertices and faces
    int num_vertex = vertices.size();
    int num_faces = faces.size();

    std::cout << "Total vertices: " << num_vertex << "\n";
    std::cout << "Total faces: " << num_faces << "\n";

    // Store vertices and faces in the class members
    ObjLoader::vertices = vertices;
    ObjLoader::faces = faces;
    ObjLoader::numVertices = num_vertex;
    ObjLoader::numFaces = num_faces;
}

void ObjLoader::loadFaces() {
    calculateNormals();
    calculateVertexNormals();
}

void ObjLoader::loadVertices() {
}