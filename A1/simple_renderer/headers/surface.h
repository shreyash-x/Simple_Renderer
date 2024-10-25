#pragma once

#include "common.h"
#include "texture.h"
#include "innerbvh.h"

struct Surface {
    std::vector<Vector3f> vertices, normals;
    std::vector<Vector3i> indices;
    std::vector<Vector2f> uvs;
    std::pair<Vector3f, Vector3f> aabb;
    std::vector<std::pair<Vector3f, Vector3f>> faceAABBs;
    LowLevelBVH bvh;

    bool isLight;
    uint32_t shapeIdx;

    Vector3f diffuse;
    float alpha;

    Texture diffuseTexture, alphaTexture;

    void getAABB();
    Interaction rayPlaneIntersect(Ray ray, Vector3f p, Vector3f n);
    Interaction rayTriangleIntersect(Ray ray, Vector3f v1, Vector3f v2, Vector3f v3, Vector3f n);
    Interaction rayIntersect(Ray ray, int intersectionVariant);
    Interaction rayIntersectLowLevelBVH(Ray ray, LowLevelBVHNode *node, double* t);

private:
    bool hasDiffuseTexture();
    bool hasAlphaTexture();
};

std::vector<Surface> createSurfaces(std::string pathToObj, bool isLight, uint32_t shapeIdx);