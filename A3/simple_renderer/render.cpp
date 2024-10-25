#include "render.h"

Integrator::Integrator(Scene &scene)
{
    this->scene = scene;
    this->outputImage.allocate(TextureType::UNSIGNED_INTEGER_ALPHA, this->scene.imageResolution);
}

// long long Integrator::render(int spp)
// {
//     auto startTime = std::chrono::high_resolution_clock::now();
//     for (int x = 0; x < this->scene.imageResolution.x; x++)
//     {
//         for (int y = 0; y < this->scene.imageResolution.y; y++)
//         {
//             Vector3f result(0, 0, 0);
//             for (int i = 0; i < spp; i++)
//             {
//                 Ray cameraRay = this->scene.camera.generateRay(x, y);
//                 Interaction si = this->scene.rayIntersect(cameraRay);
//                 Interaction siLight = this->scene.rayEmitterIntersect(cameraRay);

//                 if (si.didIntersect || siLight.didIntersect)
//                 {
//                     if (si.t < siLight.t)
//                     {
//                         Vector3f radiance;
//                         LightSample ls;
//                         for (Light &light : this->scene.lights)
//                         {
//                             std::tie(radiance, ls) = light.sample(&si);

//                             Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
//                             Interaction siShadow = this->scene.rayIntersect(shadowRay);

//                             if (!siShadow.didIntersect || siShadow.t > ls.d)
//                             {
//                                 result += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo));
//                             }
//                         }
//                     }
//                     else
//                     {
//                         result += siLight.emissiveColor;
//                     }
//                 }
//             }
//             result /= spp;
//             this->outputImage.writePixelColor(result, x, y);
//         }
//     }
//     auto finishTime = std::chrono::high_resolution_clock::now();

//     return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
// }

long long Integrator::renderLS(int spp)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < this->scene.imageResolution.x; x++)
    {
        for (int y = 0; y < this->scene.imageResolution.y; y++)
        {
            Vector3f result(0, 0, 0);
            Ray cameraRay = this->scene.camera.generateRay(x, y);
            Interaction si = this->scene.rayIntersect(cameraRay);
            Interaction siLight = this->scene.rayEmitterIntersect(cameraRay);

            if (si.didIntersect || siLight.didIntersect)
            {
                if (si.t < siLight.t)
                {
                    Vector3f radiance;
                    LightSample ls;
                    for (Light &light : this->scene.lights)
                    {
                        Vector3f color = Vector3f(0, 0, 0);
                        for (int i = 0; i < spp; i++)
                        {
                            std::tie(radiance, ls) = light.sample(&si);
                            Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
                            Interaction siShadow = this->scene.rayIntersect(shadowRay);

                            if (!siShadow.didIntersect || siShadow.t > ls.d)
                            {
                                color += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo));
                            }
                        }
                        result += color / spp;
                    }
                }
                else
                {
                    result += siLight.emissiveColor;
                }
            }
            this->outputImage.writePixelColor(result, x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

long long Integrator::renderHS(int spp, int samplingStrategy)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < this->scene.imageResolution.x; x++)
    {
        for (int y = 0; y < this->scene.imageResolution.y; y++)
        {
            Vector3f result(0, 0, 0);
            Ray cameraRay = this->scene.camera.generateRay(x, y);
            Interaction si = this->scene.rayIntersect(cameraRay);
            Interaction siLight = this->scene.rayEmitterIntersect(cameraRay);

            if (si.didIntersect || siLight.didIntersect)
            {
                if (si.t < siLight.t)
                {
                    int i = 0;
                    int j = 0;
                    Vector3f color = Vector3f(0, 0, 0);
                    for (int i = 0; i < spp; i++)
                    {
                        Vector3f localDir;
                        if (samplingStrategy == 0)
                        {
                            localDir = uniformSampleHemisphere();
                        }
                        else
                        {
                            localDir = cosineSampleHemisphere();
                        }
                        Vector3f globalDir = si.toWorld(localDir);
                        Ray shadowRay(si.p + 1e-3f * si.n, globalDir);
                        Interaction siShadow = this->scene.rayIntersect(shadowRay);
                        Interaction siShadowLight = this->scene.rayEmitterIntersect(shadowRay);

                        if (siShadow.t > siShadowLight.t && siShadowLight.didIntersect)
                        {
                            i++;
                            color += si.bsdf->eval(&si, -globalDir) * siShadowLight.emissiveColor * std::abs(Dot(si.n, -globalDir));
                        }
                        else
                        {
                            j++;
                        }
                    }
                    result += 2 * M_PI * color / spp;
                    // std::cout << "i: " << i << " j: " << j << std::endl;
                }
                else
                {
                    result += siLight.emissiveColor;
                }
            }
            this->outputImage.writePixelColor(result, x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

long long Integrator::render(int spp, int samplingStrategy)
{
    if (samplingStrategy == 2)
    {
        return this->renderLS(spp);
    }
    else
    {
        return this->renderHS(spp, samplingStrategy);
    }
}

int main(int argc, char **argv)
{
    if (argc != 5)
    {
        std::cerr << "Usage: ./render <scene_config> <out_path> <num_samples> <sampling_strategy>";
        return 1;
    }
    Scene scene(argv[1]);

    Integrator rayTracer(scene);
    int spp = atoi(argv[3]);
    int samplingStrategy = atoi(argv[4]);
    auto renderTime = rayTracer.render(spp, samplingStrategy);

    std::cout << "Render Time: " << std::to_string(renderTime / 1000.f) << " ms" << std::endl;
    rayTracer.outputImage.save(argv[2]);

    return 0;
}
