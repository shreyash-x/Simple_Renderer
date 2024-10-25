#include "render.h"

Integrator::Integrator(Scene &scene)
{
    this->scene = scene;
    this->outputImage.allocate(TextureType::UNSIGNED_INTEGER_ALPHA, this->scene.imageResolution);
}

long long Integrator::render(int interpolationVariant)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < this->scene.imageResolution.x; x++)
    {
        for (int y = 0; y < this->scene.imageResolution.y; y++)
        {
            Ray cameraRay = this->scene.camera.generateRay(x, y);
            Interaction si = this->scene.rayIntersect(cameraRay, interpolationVariant);

            if (si.didIntersect)
            {
                // this->outputImage.writePixelColor(0.5f * (si.n + Vector3f(1.f, 1.f, 1.f)), x, y);
                Vector3f pixelColor = Vector3f(0.f, 0.f, 0.f);
                for (auto &light : this->scene.lights)
                {
                    Vector3f lightDirection;
                    float distanceToLight;
                    if (light.type == LightType::POINT_LIGHT)
                    {
                        lightDirection = Normalize(light.position - si.p);
                        distanceToLight = (light.position - si.p).Length();
                    }
                    else if (light.type == LightType::DIRECTIONAL_LIGHT)
                    {
                        lightDirection = Normalize(light.direction);
                        distanceToLight = 1e30f;
                    }

                    Ray shadowRay(si.p + si.n * 1e-3f, lightDirection, distanceToLight);
                    Interaction shadowHit = this->scene.rayIntersect(shadowRay, interpolationVariant);
                    if (!shadowHit.didIntersect)
                    {
                        Vector3f cx = si.color;
                        if (light.type == LightType::POINT_LIGHT)
                        {
                            float rSquare = (light.position - si.p).LengthSquared();
                            float wCosTheta = Dot(lightDirection, Normalize(si.n));
                            if (wCosTheta < 0.f)
                                wCosTheta = 0.f;
                            Vector3f BRDF = cx / M_PI;
                            pixelColor += (light.radiance * BRDF * wCosTheta) / rSquare;
                        }
                        else if (light.type == LightType::DIRECTIONAL_LIGHT)
                        {
                            float wCosTheta = Dot(light.direction, Normalize(si.n));
                            if (wCosTheta < 0.f)
                                wCosTheta = 0.f;
                            Vector3f BRDF = cx / M_PI;
                            pixelColor += light.radiance * BRDF * wCosTheta;
                        }
                    }
                }
                this->outputImage.writePixelColor(pixelColor, x, y);
            }
            else
                this->outputImage.writePixelColor(Vector3f(0.f, 0.f, 0.f), x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        std::cerr << "Usage: ./render <scene_config> <out_path> <interpolation_variant>";
        return 1;
    }
    Scene scene(argv[1]);
    int interpolationVariant = std::stoi(argv[3]);
    Integrator rayTracer(scene);
    auto renderTime = rayTracer.render(interpolationVariant);

    std::cout << "Render Time: " << std::to_string(renderTime / 1000.f) << " ms" << std::endl;
    rayTracer.outputImage.save(argv[2]);

    return 0;
}
