#pragma once

#include "scene.h"

struct Integrator {
    Integrator(Scene& scene);

    long long renderLS(int spp);
    long long renderHS(int spp, int samplingStrategy);
    long long render(int spp, int samplingStrategy);

    Scene scene;
    Texture outputImage;
};