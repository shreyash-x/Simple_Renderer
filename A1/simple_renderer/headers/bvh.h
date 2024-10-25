#pragma once

#include "surface.h"

struct BVHNode {
    std::vector<int> surfaceIndices;
    std::pair<Vector3f, Vector3f> aabb;
    BVHNode *left, *right;
    int childCount;
    bool isLeaf() { return childCount == 0; }
};

struct BVH {
    BVHNode root;
    std::vector<Surface> surfaces;

    BVH() {};
    BVH(std::vector<Surface> surfaces);
    void build(BVHNode* node);
    std::pair<Vector3f, Vector3f> getBoundingBox(BVHNode* node);
};