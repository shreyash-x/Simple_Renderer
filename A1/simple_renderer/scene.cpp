#include "scene.h"

Scene::Scene(std::string sceneDirectory, std::string sceneJson)
{
    nlohmann::json sceneConfig;
    try
    {
        sceneConfig = nlohmann::json::parse(sceneJson);
    }
    catch (std::runtime_error e)
    {
        std::cerr << "Could not parse json." << std::endl;
        exit(1);
    }

    this->parse(sceneDirectory, sceneConfig);
}

Scene::Scene(std::string pathToJson)
{
    std::string sceneDirectory;

#ifdef _WIN32
    const size_t last_slash_idx = pathToJson.rfind('\\');
#else
    const size_t last_slash_idx = pathToJson.rfind('/');
#endif

    if (std::string::npos != last_slash_idx)
    {
        sceneDirectory = pathToJson.substr(0, last_slash_idx);
    }

    nlohmann::json sceneConfig;
    try
    {
        std::ifstream sceneStream(pathToJson.c_str());
        sceneStream >> sceneConfig;
    }
    catch (std::runtime_error e)
    {
        std::cerr << "Could not load scene .json file." << std::endl;
        exit(1);
    }

    this->parse(sceneDirectory, sceneConfig);
}

void Scene::parse(std::string sceneDirectory, nlohmann::json sceneConfig)
{
    // Output
    try
    {
        auto res = sceneConfig["output"]["resolution"];
        this->imageResolution = Vector2i(res[0], res[1]);
    }
    catch (nlohmann::json::exception e)
    {
        std::cerr << "\"output\" field with resolution, filename & spp should be defined in the scene file." << std::endl;
        exit(1);
    }

    // Cameras
    try
    {
        auto cam = sceneConfig["camera"];

        this->camera = Camera(
            Vector3f(cam["from"][0], cam["from"][1], cam["from"][2]),
            Vector3f(cam["to"][0], cam["to"][1], cam["to"][2]),
            Vector3f(cam["up"][0], cam["up"][1], cam["up"][2]),
            float(cam["fieldOfView"]),
            this->imageResolution);
    }
    catch (nlohmann::json::exception e)
    {
        std::cerr << "No camera(s) defined. Atleast one camera should be defined." << std::endl;
        exit(1);
    }

    // Surface
    try
    {
        auto surfacePaths = sceneConfig["surface"];

        uint32_t surfaceIdx = 0;
        for (std::string surfacePath : surfacePaths)
        {
            surfacePath = sceneDirectory + "/" + surfacePath;

            auto surf = createSurfaces(surfacePath, /*isLight=*/false, /*idx=*/surfaceIdx);
            this->surfaces.insert(this->surfaces.end(), surf.begin(), surf.end());

            surfaceIdx = surfaceIdx + surf.size();
        }

        // Generate AABBs
        for (auto &surface : this->surfaces)
        {
            surface.getAABB();
            surface.bvh = LowLevelBVH(surface.vertices, surface.indices, surface.faceAABBs);
        }

        // Build BVHh
        this->bvh = BVH(this->surfaces);
        auto finishTime = std::chrono::high_resolution_clock::now();
    }
    catch (nlohmann::json::exception e)
    {
        std::cout << "No surfaces defined." << std::endl;
    }
}

Interaction Scene::rayIntersect(Ray &ray, int intersectionVariant)
{
    Interaction siFinal;
    if (intersectionVariant == 0 || intersectionVariant == 1)
    {
        int surfaceIdx = -1;
        for (auto &surface : this->surfaces)
        {
            surfaceIdx++;
            if (intersectionVariant == 1)
            {
                if (!rayIntersectAABB(ray, surface.aabb))
                    continue;
            }
            Interaction si = surface.rayIntersect(ray, intersectionVariant);
            if (si.t <= ray.t)
            {
                siFinal = si;
                ray.t = si.t;
            }
        }
    }
    else
    {
        siFinal = rayIntersectBVH(ray, &this->bvh.root, intersectionVariant);
    }

    return siFinal;
}

Interaction Scene::rayIntersectBVH(Ray &ray, BVHNode *node, int intersectionVariant)
{
    if (node->isLeaf())
    {
        Interaction siFinal;
        if (rayIntersectAABB(ray, node->aabb))
        {
            for (int surfaceIdx : node->surfaceIndices)
            {
                Surface &surface = this->surfaces[surfaceIdx];
                if (!rayIntersectAABB(ray, surface.aabb))
                    continue;
                Interaction si = surface.rayIntersect(ray, intersectionVariant);
                this->count++;

                if (si.t <= ray.t)
                {
                    siFinal = si;
                    ray.t = si.t;
                }
            }
        }

        return siFinal;
    }
    else
    {
        Interaction siLeft, siRight;

        if (rayIntersectAABB(ray, node->left->aabb))
            siLeft = rayIntersectBVH(ray, node->left, intersectionVariant);

        if (rayIntersectAABB(ray, node->right->aabb))
            siRight = rayIntersectBVH(ray, node->right, intersectionVariant);

        if (siLeft.t < siRight.t)
            return siLeft;
        else
            return siRight;
    }
}