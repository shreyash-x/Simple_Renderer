#include "innerbvh.h"

LowLevelBVH::LowLevelBVH(std::vector<Vector3f> vertices, std::vector<Vector3i> indices, std::vector<std::pair<Vector3f, Vector3f>> aaBBs)
{
    this->vertices = vertices;
    this->indices = indices;
    this->AABBs = aaBBs;
    this->root.faceIndices = std::vector<int>(indices.size());
    for (int i = 0; i < indices.size(); i++)
    {
        this->root.faceIndices[i] = i;
    }
    this->build(&this->root);
}

void LowLevelBVH::build(LowLevelBVHNode *node)
{
    std::pair<Vector3f, Vector3f> aabb = getBoundingBox(node);
    node->aabb = aabb;

    if (node->faceIndices.size() <= 1)
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
    std::vector<int> leftFaces;
    std::vector<int> rightFaces;
    for (int faceIdx : node->faceIndices)
    {
        Vector3i face = this->indices[faceIdx];
        Vector3f centroid = (this->vertices[face.x] + this->vertices[face.y] + this->vertices[face.z]) / 3.f;
        if (centroid[splitAxis] < splitPoint)
        {
            leftFaces.push_back(faceIdx);
        }
        else
        {
            rightFaces.push_back(faceIdx);
        }
    }

    // If we can't split, make this a leaf node
    if (leftFaces.size() == 0 || rightFaces.size() == 0)
    {
        node->childCount = 0;
        return;
    }

    // Create left and right nodes
    if (leftFaces.size() > 0)
    {
        node->childCount++;
        node->left = new LowLevelBVHNode();
        node->left->faceIndices = leftFaces;
        build(node->left);
    }

    if (rightFaces.size() > 0)
    {
        node->childCount++;
        node->right = new LowLevelBVHNode();
        node->right->faceIndices = rightFaces;
        build(node->right);
    }
}

std::pair<Vector3f, Vector3f> LowLevelBVH::getBoundingBox(LowLevelBVHNode *node)
{
    std::pair<Vector3f, Vector3f> aabb;
    Vector3f min = this->vertices[this->indices[node->faceIndices[0]].x];
    Vector3f max = this->vertices[this->indices[node->faceIndices[0]].x];
    for (int faceIdx : node->faceIndices)
    {
        std::pair<Vector3f, Vector3f> faceAABB = this->AABBs[faceIdx];
        min = Vector3f(std::min(min.x, faceAABB.first.x), std::min(min.y, faceAABB.first.y), std::min(min.z, faceAABB.first.z));
        max = Vector3f(std::max(max.x, faceAABB.second.x), std::max(max.y, faceAABB.second.y), std::max(max.z, faceAABB.second.z));
    }
    aabb.first = min;
    aabb.second = max;
    return aabb;
}

bool rayIntersectAABB(Ray ray, std::pair<Vector3f, Vector3f> aabb)
{
    // "slab test"
    Vector3f aabbMin = aabb.first;
    Vector3f aabbMax = aabb.second;
    float tx1 = (aabbMin.x - ray.o.x) / ray.d.x, tx2 = (aabbMax.x - ray.o.x) / ray.d.x;
    float tmin = std::min(tx1, tx2), tmax = std::max(tx1, tx2);

    float ty1 = (aabbMin.y - ray.o.y) / ray.d.y, ty2 = (aabbMax.y - ray.o.y) / ray.d.y;
    tmin = std::max(tmin, std::min(ty1, ty2));
    tmax = std::min(tmax, std::max(ty1, ty2));

    float tz1 = (aabbMin.z - ray.o.z) / ray.d.z, tz2 = (aabbMax.z - ray.o.z) / ray.d.z;
    tmin = std::max(tmin, std::min(tz1, tz2));
    tmax = std::min(tmax, std::max(tz1, tz2));

    return tmax >= tmin && tmin < ray.t && tmax > 0;
}