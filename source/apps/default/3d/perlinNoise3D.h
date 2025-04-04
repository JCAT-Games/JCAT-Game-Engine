#ifndef PERLIN_NOISE_3D_H
#define PERLIN_NOISE_3D_H

#include <array>
#include <cmath>

namespace JCAT {
    class PerlinNoise3D {
        public:
            PerlinNoise3D(unsigned int seed);
            ~PerlinNoise3D();

            static float generate3DPerlinNoise(PerlinNoise3D& object, float x, float y, float z, float scale, float amplitude);
        private:
            std::array<int, 512> permutation;
    };
};

#endif