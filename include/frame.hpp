#pragma once

#include <glm/fwd.hpp>
#include <glm/glm.hpp>

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

//https://terra-1-g.djicdn.com/65c028cd298f4669a7f0e40e50ba1131/Download/Avia/readme_EN.md
inline glm::vec3 GetColorMap(uint16_t color) {
    switch (color) {
        case 1: return glm::vec3(0.0f, 1.0f, 0.0f); // Green car
        case 2: return glm::vec3(0.0f, 0.0f, 1.0f); // Blue truck
        case 3: return glm::vec3(1.0f, 1.0f, 0.0f); // Yellow bus
        case 4: return glm::vec3(1.0f, 0.5f, 0.0f); // Orange bicycle
        case 5: return glm::vec3(0.5f, 0.0f, 0.5f); // Purple motor
        case 6: return glm::vec3(0.0f, 1.0f, 1.0f); // Cyan pedestrian
        case 7: return glm::vec3(1.0f, 0.0f, 1.0f); // Magenta dog
        case 8: return glm::vec3(0.5f, 0.5f, 0.5f); // Grey road
        case 9: return glm::vec3(0.5f, 1.0f, 0.0f); // Lime Green ground
        case 10: return glm::vec3(0.0f, 0.5f, 1.0f); // Cerulean Blue building
        case 11: return glm::vec3(1.0f, 0.75f, 0.0f); // Gold fence
        case 12: return glm::vec3(0.63f, 0.32f, 0.18f); // Brown tree
        case 13: return glm::vec3(0.5f, 0.2f, 0.7f); // Violet pole
        case 14: return glm::vec3(0.9f, 0.4f, 0.3f); // Salmon greenbelt
        default: return glm::vec3(1.0f, 1.0f, 1.0f); // White unknown
    }
}

struct TPoint {
    double X, Y, Z;
    int MotionState, Type, LidarNumber;
};

struct TAnnotation {
    uint64_t TrackingId;
    std::string Type;
    double X, Y, Z, Length, Width, Height, Yaw;
};

using TPointsList = std::vector<TPoint>;
using TAnnotationsList = std::vector<TAnnotation>;

struct TFrame {

    TPointsList Points;
    TAnnotationsList Annotations;

    TFrame(TPointsList&& points, TAnnotationsList&& annotations)
        : Points(points)
        , Annotations(annotations)
    {}

};

using TFramesList = std::vector<TFrame>;

class TFrameReader {
private:
    std::string _DatasetPath;

public:
    TFrameReader(const std::string& folder)
        : _DatasetPath(folder)
    {
    }

    TFramesList read() {
        TFramesList frames;
        for (const auto& filename : GetListOfFiles()) {
            frames.emplace_back(
                ReadPoints(filename),
                TAnnotationsList()
            );
        }
        return frames;
    }

protected:
    std::vector<std::string> GetListOfFiles() {
        std::vector<std::string> filenames;
        for (const auto& entry : std::filesystem::directory_iterator(_DatasetPath + "/points")) {
            filenames.emplace_back(entry.path().filename().string());
        }
        std::sort(filenames.begin(), filenames.end());
        return filenames;
    }

    TPointsList ReadPoints(const std::string& filename) {
        std::cout << "read points: " << filename << std::endl;
        std::ifstream fileStream(_DatasetPath + "/points/" + filename);
        if (!fileStream.is_open()) {
            std::cerr << "unable to open file: " << filename << std::endl;;
            {};
        }
        
        TPointsList points;
        points.reserve(140000);
        std::string line;
        while (std::getline(fileStream, line)) {
            std::istringstream lineStream(line);
            TPoint point;
            char comma;
            if (
                lineStream >> point.X >> comma
                           >> point.Y >> comma
                           >> point.Z >> comma
                           >> point.MotionState >> comma
                           >> point.Type >> comma
                           >> point.LidarNumber
            ) {
                points.emplace_back(std::move(point));
            }
        }
        fileStream.close();
        return points;
    }

    TAnnotationsList ReadAnnotations(const std::string& filename) {
        std::cout << "read annotations: " << filename << std::endl;
        std::ifstream fileStream(_DatasetPath + "/annotations/" + filename);
        if (!fileStream.is_open()) {
            std::cerr << "unable to open file: " << filename << std::endl;
            {};
        }
        
        TAnnotationsList annotations;
        std::string line;
        while (std::getline(fileStream, line)) {
            std::istringstream lineStream(line);
            TAnnotation annotation;
            char comma;
            if (
                lineStream >> annotation.TrackingId >> comma
                           >> annotation.Type >> comma
                           >> annotation.X >> comma
                           >> annotation.Y >> comma
                           >> annotation.Z >> comma
                           >> annotation.Length >> comma
                           >> annotation.Width >> comma
                           >> annotation.Height >> comma
                           >> annotation.Yaw
            ) {
                annotations.emplace_back(std::move(annotation));
            }
        }
        fileStream.close();
        return annotations;
    }
};
