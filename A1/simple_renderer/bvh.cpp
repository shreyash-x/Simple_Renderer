#include "bvh.h"

BVH::BVH(std::vector<Surface> surfaces)
{
    this->root = BVHNode();
    this->surfaces = surfaces;
    this->root.surfaceIndices = std::vector<int>(surfaces.size());
    for (int i = 0; i < surfaces.size(); i++)
    {
        this->root.surfaceIndices[i] = i;
    }
    this->build(&this->root);
}

void BVH::build(BVHNode *node)
{

    std::pair<Vector3f, Vector3f> aabb = getBoundingBox(node);
    node->aabb = aabb;

    if (node->surfaceIndices.size() <= 1)
    {
        node->childCount = 0;
        return;
    }

    Vector3f extent = aabb.second - aabb.first;
    int splitAxis = 0; // 0 = x, 1 = y, 2 = zh
    if (extent.y > extent.x && extent.y > extent.z)
    {
        splitAxis = 1;
    }
    else if (extent.z > extent.x && extent.z > extent.y)
    {
        splitAxis = 2;
    }
    float splitPoint = (aabb.first[splitAxis] + aabb.second[splitAxis]) / 2.f;
    // Calculate object centroids and determine left and right
    std::vector<int> leftSurfaces;
    std::vector<int> rightSurfaces;
    for (int surfaceIdx : node->surfaceIndices)
    {
        Vector3f centroid = (this->surfaces[surfaceIdx].aabb.first + this->surfaces[surfaceIdx].aabb.second) / 2.f;
        if (centroid[splitAxis] < splitPoint)
        {
            leftSurfaces.push_back(surfaceIdx);
        }
        else
        {
            rightSurfaces.push_back(surfaceIdx);
        }
    }

    // If we can't split, make this a leaf node
    if (leftSurfaces.size() == 0 || rightSurfaces.size() == 0)
    {
        node->childCount = 0;
        return;
    }

    // Create left and right nodes
    if (leftSurfaces.size() > 0)
    {
        node->childCount++;
        node->left = new BVHNode();
        node->left->surfaceIndices = leftSurfaces;
        build(node->left);
    }

    if (rightSurfaces.size() > 0)
    {
        node->childCount++;
        node->right = new BVHNode();
        node->right->surfaceIndices = rightSurfaces;
        build(node->right);
    }
}

std::pair<Vector3f, Vector3f> BVH::getBoundingBox(BVHNode *node)
{
    Surface surfaceInit = this->surfaces[node->surfaceIndices[0]];
    Vector3f min = surfaceInit.aabb.first;
    Vector3f max = surfaceInit.aabb.second;
    for (int surfaceIdx : node->surfaceIndices)
    {
        std::pair<Vector3f, Vector3f> aabb = this->surfaces[surfaceIdx].aabb;
        Vector3f minAabb = aabb.first;
        Vector3f maxAabb = aabb.second;

        min.x = std::min(min.x, minAabb.x);
        min.y = std::min(min.y, minAabb.y);
        min.z = std::min(min.z, minAabb.z);

        max.x = std::max(max.x, maxAabb.x);
        max.y = std::max(max.y, maxAabb.y);
        max.z = std::max(max.z, maxAabb.z);
    }

    return std::make_pair(min, max);
}
