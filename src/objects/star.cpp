#include <iostream>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <regex>
#include "../rasterizer.hpp"

void Star::loadVertices() {
    std::ifstream file("resources/knot.asc");

    if (!file.is_open()) {
        std::cerr << "Failed to open file.\n";
        return;
    }

    std::string line;
    bool readingVertices = false;
    bool readingFaces = false;

    std::vector<slib::vec3> vertices;
    std::vector<Face> faces;

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
                slib::vec3 vertex;
                std::regex vertexRegex(R"(Vertex\s+\d+:\s+X:\s+([-.\dEe]+)\s+Y:\s+([-.\dEe]+)\s+Z:\s+([-.\dEe]+))");
                std::smatch match;

                if (std::regex_search(line, match, vertexRegex)) {
                    vertex.x = std::stof(match[1]);
                    vertex.y = std::stof(match[2]);
                    vertex.z = std::stof(match[3]);

                    vertices.push_back(vertex);
                }
            }
        }

        if (readingFaces) {
            if (line.find("Face") != std::string::npos) {
                // Example line: Face 0:    A:0 B:1 C:2 AB:1 BC:1 CA:0
                Face face;
                std::regex faceRegex(R"(Face\s+\d+:\s+A:(\d+)\s+B:(\d+)\s+C:(\d+))");
                std::smatch match;

                if (std::regex_search(line, match, faceRegex)) {
                    face.vertex1 = std::stoi(match[3]);
                    face.vertex2 = std::stoi(match[2]);
                    face.vertex3 = std::stoi(match[1]);
                    face.material = { 0xff0058fc, 0xff0058fc, 0xff0058fc, getMaterialProperties(MaterialType::Metal) };

                    faces.push_back(face);
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
    Star::vertices = vertices;
    Star::faces = faces;
    Star::numVertices = num_vertex;
    Star::numFaces = num_faces;
}

void Star::loadFaces() {
    calculateNormals();
    calculateVertexNormals();
}