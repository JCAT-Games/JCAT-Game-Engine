#include <vector>
#include <random>
#include <numeric>
#include <algorithm>

#include "./apps/default/3d/perlinNoise3D.h"

namespace JCAT {
    PerlinNoise3D::PerlinNoise3D(unsigned int seed = 0) {
        std::vector<int> p(256);

        // Fill with values from 0 to 255
        std::iota(p.begin(), p.end(), 0);

        std::default_random_engine engine(seed);
        std::shuffle(p.begin(), p.end(), engine);

        // Duplicate the permutation vector
        for (int i = 0; i < 256; ++i) {
            permutation[i] = permutation[i + 256] = p[i];
        }
    }

    PerlinNoise3D::~PerlinNoise3D() {}

    float PerlinNoise3D::generate3DPerlinNoise(PerlinNoise3D& object, float x, float y, float z, float scale, float amplitude) {
        // Fade function for smooth interpolation
        auto fade = [](float t) { return t * t * t * (t * (t * 6 - 15) + 10); };

        // Linear interpolation
        auto lerp = [](float a, float b, float t) { return a + t * (b - a); };

        // Hash function for gradient indexing
        auto hash = [](const std::array<int, 512>& permutation, int x, int y, int z) {
            return permutation[(permutation[(permutation[x & 255] + y) & 255] + z) & 255];
        };

        // Gradient calculation based on hash
        auto grad = [](int hash, float x, float y, float z) {
            int h = hash & 15;
            float u = h < 8 ? x : y;
            float v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
            
            return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
        };

        // Scale the coordinates
        x *= scale;
        y *= scale;
        z *= scale;

        // Find the lattice points
        int X = static_cast<int>(floor(x)) & 255;
        int Y = static_cast<int>(floor(y)) & 255;
        int Z = static_cast<int>(floor(z)) & 255;

        // Compute relative positions in the lattice
        x -= floor(x);
        y -= floor(y);
        z -= floor(z);

        // Compute fade curves
        float u = fade(x);
        float v = fade(y);
        float w = fade(z);

        // Hash the corners of the lattice cube and calculate dot products
        int aaa = hash(object.permutation, X, Y, Z);
        int aba = hash(object.permutation, X, Y + 1, Z);
        int aab = hash(object.permutation, X, Y, Z + 1);
        int abb = hash(object.permutation, X, Y + 1, Z + 1);
        int baa = hash(object.permutation, X + 1, Y, Z);
        int bba = hash(object.permutation, X + 1, Y + 1, Z);
        int bab = hash(object.permutation, X + 1, Y, Z + 1);
        int bbb = hash(object.permutation, X + 1, Y + 1, Z + 1);

        // Perform interpolation
        float result = lerp(w,
            lerp(v, lerp(u, grad(aaa, x, y, z), grad(baa, x - 1, y, z)),
                lerp(u, grad(aba, x, y - 1, z), grad(bba, x - 1, y - 1, z))),
            lerp(v, lerp(u, grad(aab, x, y, z - 1), grad(bab, x - 1, y, z - 1)),
                lerp(u, grad(abb, x, y - 1, z - 1), grad(bbb, x - 1, y - 1, z - 1)))
        );

        // Scale result to [0, amplitude] range
        return (result + 1.0f) / 2.0f * amplitude;
    }
};