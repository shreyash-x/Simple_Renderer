#pragma once

#include "common.h"

struct LowLevelBVHNode {
    std::vector<int> faceIndices;
    std::pair<Vector3f, Vector3f> aabb;
    LowLevelBVHNode *left, *right;
    int childCount;
    bool isLeaf() { return childCount == 0; }
};

struct LowLevelBVH {
    LowLevelBVHNode root;
    std::vector<Vector3f> vertices;
    std::vector<Vector3i> indices;
    std::vector<std::pair<Vector3f, Vector3f>> AABBs;


    LowLevelBVH() {};
    LowLevelBVH(std::vector<Vector3f> vertices, std::vector<Vector3i> indices, std::vector<std::pair<Vector3f, Vector3f>> aaBBs);
    void build(LowLevelBVHNode* node);
    std::pair<Vector3f, Vector3f> getBoundingBox(LowLevelBVHNode* node);
};

bool rayIntersectAABB(Ray ray, std::pair<Vector3f, Vector3f> aabb);