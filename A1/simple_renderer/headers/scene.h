#pragma once

#include "camera.h"
#include "surface.h"
#include "bvh.h"

struct Scene {
    std::vector<Surface> surfaces;
    Camera camera;
    Vector2i imageResolution;
    BVH bvh;

    int count = 0;

    Scene() {};
    Scene(std::string sceneDirectory, std::string sceneJson);
    Scene(std::string pathToJson);
    
    void parse(std::string sceneDirectory, nlohmann::json sceneConfig);

    Interaction rayIntersect(Ray& ray, int intersectionVariant);
    Interaction rayIntersectBVH(Ray &ray, BVHNode *node, int intersectionVariant);
};