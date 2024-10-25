#pragma once

#include "vec.h"

// Forward declaration of BSDF class
class BSDF;

struct Interaction {
    // Position of interaction
    Vector3f p;
    // Normal of the surface at interaction
    Vector3f n;
    // The uv co-ordinates at the intersection point
    Vector2f uv;
    // The viewing direction in local shading frame
    Vector3f wi; 
    // Distance of intersection point from origin of the ray
    float t = 1e30f; 
    // Used for light intersection, holds the radiance emitted by the emitter.
    Vector3f emissiveColor = Vector3f(0.f, 0.f, 0.f);
    // BSDF at the shading point
    BSDF* bsdf;
    // Vectors defining the orthonormal basis
    Vector3f a, b, c;

    bool didIntersect = false;

    Vector3f toLocal(Vector3f w) {
        // TODO: Implement this
        Vector3f globalDir;
        globalDir.x = w.x * a.x + w.y * a.y + w.z * a.z;
        globalDir.y = w.x * b.x + w.y * b.y + w.z * b.z;
        globalDir.z = w.x * c.x + w.y * c.y + w.z * c.z;
        return globalDir;
    }

    Vector3f toWorld(Vector3f w) {
        // TODO: Implement this
        Vector3f localDir;
        localDir.x = w.x * a.x + w.y * b.x + w.z * c.x;
        localDir.y = w.x * a.y + w.y * b.y + w.z * c.y;
        localDir.z = w.x * a.z + w.y * b.z + w.z * c.z;
        return localDir;
    }
};