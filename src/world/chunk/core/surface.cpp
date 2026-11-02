#include "world/chunk/core/surface.hpp"

#include <algorithm>
#include <cmath>

#include "math/utils.hpp"

namespace robot::world::chunk {

Surface SurfaceResolver::resolve(const SurfaceInput& input) const {
    static constexpr float SNOW_MAX_SLOPE = 0.85f;  // 坡度太陡積不了雪
    static constexpr float ROCK_MIN_SLOPE = 1.2f;   // 坡度太陡會露出岩石

    if (input.height <= input.waterLevel) {
        if (isFrozen(input.biome)) {
            return Surface::Ice;
        } else {
            return Surface::Water;
        }
    }

    const float rockSlope = isFrozen(input.biome) ? SNOW_MAX_SLOPE : ROCK_MIN_SLOPE;
    if (input.slope >= rockSlope) {
        return Surface::Stone;
    }

    return defaultSurface(input.biome);
}

bool SurfaceResolver::isFrozen(const Biome& biome) const {
    switch (biome) {
    case Biome::FrozenOcean:
    case Biome::SnowyPlains:
    case Biome::SnowyTaiga:
    case Biome::SnowyPeaks:
        return true;
    default:
        return false;
    }
}

Surface SurfaceResolver::defaultSurface(const Biome& biome) const {
    switch (biome) {
    case Biome::Ocean:
    case Biome::FrozenOcean:
        return Surface::Water;
    case Biome::StoneShore:
    case Biome::StonyPeaks:
        return Surface::Stone;
    case Biome::Beach:
    case Biome::Desert:
        return Surface::Sand;
    case Biome::SnowyPlains:
    case Biome::SnowyTaiga:
    case Biome::SnowyPeaks:
        return Surface::Snow;
    default:
        return Surface::Grass;
    }
}

}  // namespace robot::world::chunk
