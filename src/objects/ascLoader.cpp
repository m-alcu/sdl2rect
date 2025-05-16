#include <iostream>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <regex>
#include <cstdint>
#include "ascLoader.hpp"

void AscLoader::setup(const std::string& filename) {
    loadVertices(filename);
    loadFaces();
    calculateNormals();
    calculateVertexNormals();
}

void AscLoader::loadVertices(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open file.\n";
        return;
    }

    std::string line;
    bool readingVertices = false;
    bool readingFaces = false;

    std::vector<VertexData> vertices;
    std::vector<FaceData> faces;

    MaterialProperties properties = getMaterialProperties(MaterialType::Metal);

    slib::material material{};
    material.Ka = { properties.k_a * 0x00, properties.k_a * 0x58, properties.k_a * 0xfc };
    material.Kd = { properties.k_d * 0x00, properties.k_d * 0x58, properties.k_d * 0xfc }; 
    material.Ks = { properties.k_s * 0x00, properties.k_s * 0x58, properties.k_s * 0xfc };
    material.Ns = properties.shininess;
    materials.insert({"blue", material});

    material.Ka = { properties.k_a * 0xff, properties.k_a * 0xff, properties.k_a * 0xff };
    material.Kd = { properties.k_d * 0xff, properties.k_d * 0xff, properties.k_d * 0xff };
    material.Ks = { properties.k_s * 0xff, properties.k_s * 0xff, properties.k_s * 0xff };
    material.Ns = properties.shininess;
    materials.insert({"white", material});          

    while (std::getline(file, line)) {
        // Remove leading/trailing spaces
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        // Skip empty lines
        if (line.empty())
            continue;

        // Detect start of vertex list
        if (line.find("Vertex list:") != std::string::npos) {
            readingVertices = true;
            readingFaces = false;
            continue;
        }

        // Detect start of face list
        if (line.find("Face list:") != std::string::npos) {
            readingVertices = false;
            readingFaces = true;
            continue;
        }

        if (readingVertices) {
            if (line.find("Vertex") != std::string::npos) {
                // Example line: Vertex 0:  X: -95     Y: 0     Z: 0
                VertexData vertexData;
                std::regex vertexRegex(R"(Vertex\s+\d+:\s+X:\s+([-.\dEe]+)\s+Y:\s+([-.\dEe]+)\s+Z:\s+([-.\dEe]+))");
                std::smatch match;

                if (std::regex_search(line, match, vertexRegex)) {
                    vertexData.vertex.x = std::stof(match[1]);
                    vertexData.vertex.y = std::stof(match[2]);
                    vertexData.vertex.z = std::stof(match[3]);

                    vertices.push_back(vertexData);
                }
            }
        }


        if (readingFaces) {
            if (line.find("Face") != std::string::npos) {
                // Example line: Face 0:    A:0 B:1 C:2 AB:1 BC:1 CA:0
                FaceData faceData;
                std::regex faceRegex(R"(Face\s+\d+:\s+A:(\d+)\s+B:(\d+)\s+C:(\d+))");
                std::smatch match;

                if (std::regex_search(line, match, faceRegex)) {
                    faceData.face.vertex1 = std::stoi(match[1]);
                    faceData.face.vertex2 = std::stoi(match[2]);
                    faceData.face.vertex3 = std::stoi(match[3]);
                    faceData.face.materialKey = "blue"; // Default material key
                    faces.push_back(faceData);
                }
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
    AscLoader::vertexData = vertices;
    AscLoader::faceData = faces;
    AscLoader::numVertices = num_vertex;
    AscLoader::numFaces = num_faces;
}

void AscLoader::loadFaces() {
    calculateNormals();
    calculateVertexNormals();
}

void AscLoader::loadVertices() {
}