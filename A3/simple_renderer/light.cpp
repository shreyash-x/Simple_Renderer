#include "light.h"

Light::Light(LightType type, nlohmann::json config)
{
    switch (type)
    {
    case LightType::POINT_LIGHT:
        this->position = Vector3f(config["location"][0], config["location"][1], config["location"][2]);
        break;
    case LightType::DIRECTIONAL_LIGHT:
        this->direction = Vector3f(config["direction"][0], config["direction"][1], config["direction"][2]);
        break;
    case LightType::AREA_LIGHT:
        this->center = Vector3f(config["center"][0], config["center"][1], config["center"][2]);
        this->normal = Vector3f(config["normal"][0], config["normal"][1], config["normal"][2]);
        this->vx = Vector3f(config["vx"][0], config["vx"][1], config["vx"][2]);
        this->vy = Vector3f(config["vy"][0], config["vy"][1], config["vy"][2]);
        break;
    default:
        std::cout << "WARNING: Invalid light type detected";
        break;
    }

    this->radiance = Vector3f(config["radiance"][0], config["radiance"][1], config["radiance"][2]);
    this->type = type;
}

std::pair<Vector3f, LightSample> Light::sample(Interaction *si)
{
    LightSample ls;
    memset(&ls, 0, sizeof(ls));

    Vector3f radiance;
    switch (type)
    {
    case LightType::POINT_LIGHT:
        ls.wo = (position - si->p);
        ls.d = ls.wo.Length();
        ls.wo = Normalize(ls.wo);
        radiance = (1.f / (ls.d * ls.d)) * this->radiance;
        break;
    case LightType::DIRECTIONAL_LIGHT:
        ls.wo = Normalize(direction);
        ls.d = 1e10;
        radiance = this->radiance;
        break;
    case LightType::AREA_LIGHT:
        // TODO: Implement this
        // Sample point on area light
        float e1 = next_float();
        float e2 = next_float();
        Vector3f p = center + 2 * (e1 - 0.5f) * vx + 2 * (e2 - 0.5f) * vy;
        float Area = 4 * vx.Length() * vy.Length();
        ls.wo = (p - si->p);
        ls.d = (p - si->p).Length();
        ls.wo = Normalize(ls.wo);
        float cosThetaLight = Dot(-ls.wo, normal);
        // check if the ray is going towards the light
        if (cosThetaLight <= 0.f)
        {
            radiance = {0, 0, 0};
            break;
        }
        radiance = (cosThetaLight / (ls.d * ls.d)) * this->radiance * Area;
        break;
    }
    return {radiance, ls};
}

Interaction Light::intersectLight(Ray *ray)
{
    Interaction si;
    memset(&si, 0, sizeof(si));

    if (type == LightType::AREA_LIGHT)
    {
        float dDotN = Dot(ray->d, normal);
        if (dDotN != 0.f)
        {
            float t = -Dot((ray->o - center), normal) / dDotN;

            if (t >= 0.f)
            {
                Vector3f pointOfIntersection = ray->o + ray->d * t;
                // Check if the point of intersection is inside the rectangle
                Vector3f d = pointOfIntersection - center;
                float dx = Dot(d, vx);
                float dy = Dot(d, vy);
                float vx2 = vx.LengthSquared();
                float vy2 = vy.LengthSquared();
                if (dx >= -vx2 && dx <= vx2 && dy >= -vy2 && dy <= vy2)
                {
                    if (Dot(ray->d, normal) < 0.f)
                    {
                        si.didIntersect = true;
                        si.t = t;
                        si.n = normal;
                        si.p = pointOfIntersection;
                        si.emissiveColor = this->radiance;
                    }
                }
            }
        }
    }

    return si;
}

Vector3f uniformSampleHemisphere()
{
    float e1 = next_float();
    float e2 = next_float();

    float theta = std::acos(e1);
    float phi = 2 * M_PI * e2;

    float x = std::sin(theta) * std::cos(phi);
    float y = std::sin(theta) * std::sin(phi);
    float z = std::cos(theta);

    return Vector3f(x, y, z);
}

Vector3f cosineSampleHemisphere()
{
    float e1 = next_float();
    float e2 = next_float();

    float theta = std::acos(std::sqrt(e1));
    float phi = 2 * M_PI * e2;

    float x = std::sin(theta) * std::cos(phi);
    float y = std::sin(theta) * std::sin(phi);
    float z = std::cos(theta);

    return Vector3f(x, y, z);
}