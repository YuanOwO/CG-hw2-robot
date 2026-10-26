#include "world/chunk/core/biome.hpp"

#include <algorithm>
#include <limits>

#include "math/utils.hpp"

namespace robot::world::chunk {
namespace {

Biome snowyBiome(Biome biome) {
    switch (biome) {
    case Biome::Plains:
        return Biome::SnowyPlains;
    case Biome::Taiga:
        return Biome::SnowyTaiga;
    default:
        return biome;
    }
}

Biome thawedBiome(Biome biome) {
    switch (biome) {
    case Biome::SnowyPlains:
        return Biome::Plains;
    case Biome::SnowyTaiga:
        return Biome::Taiga;
    default:
        return biome;
    }
}

}  // namespace

BiomeClassifier::BiomeClassifier(Seed seed)
    : _temperatureJitter(seed, fnv1a32("TEMPERATURE_JITTER")),
      _humidityJitter(seed, fnv1a32("HUMIDITY_JITTER")),
      _continentJitter(seed, fnv1a32("CONTINENT_JITTER")) {}

Biome BiomeClassifier::classify(const BiomeInput& input) const {
    static constexpr float BEACH_MAX_CONTINENT = 0.04f;
    static constexpr float BEACH_MAX_HEIGHT = 0.87f;
    static constexpr float BEACH_MAX_SLOPE = 0.4f;

    static constexpr float ROCKY_RIDGE_MIN = 0.6f;

    static constexpr float TREELINE_TEMPERATURE = 0.3f;  // 溫度低於此值的山坡不會長樹
    static constexpr float FREEZING_TEMPERATURE = 0.2f;  // 溫度 0.2 以下會結冰
    static constexpr float ICED_SEA_TEMPERATURE = FREEZING_TEMPERATURE - 0.05f;

    BiomeInput adjInput = input;
    adjustBiomeInput(adjInput);

    const bool isFreezing = adjInput.temperature <= FREEZING_TEMPERATURE;

    // 海洋
    if (adjInput.height <= 0.0f && adjInput.continent < 0.0f) {
        if (adjInput.temperature <= ICED_SEA_TEMPERATURE) {
            return Biome::FrozenOcean;
        }
        return Biome::Ocean;
    }

    // 沙灘與石岸
    if (adjInput.continent <= BEACH_MAX_CONTINENT) {
        // 斷崖或陡坡是石岸，結凍的海岸也是石岸
        if (adjInput.cliffMask >= 0.5f) {
            return Biome::StoneShore;
        }
        if (adjInput.height <= BEACH_MAX_HEIGHT) {
            if (adjInput.slope > BEACH_MAX_SLOPE || isFreezing) {
                return Biome::StoneShore;
            }
            return Biome::Beach;
        }
    }

    // 沼澤 (TODO)

    // 山地 (林線以上的山坡)
    if (adjInput.mountainMask >= 0.5f && adjInput.temperature < TREELINE_TEMPERATURE) {
        if (isFreezing) {
            return Biome::SnowyPeaks;
        }
        if (adjInput.ridge >= ROCKY_RIDGE_MIN) {
            return Biome::StonyPeaks;
        }
        return Biome::AlpineMeadow;
    }

    // 根據氣候分類
    Biome biome = nearestClimateBiome(adjInput.temperature, adjInput.humidity);
    if (isFreezing) {
        biome = snowyBiome(biome);
    } else {
        biome = thawedBiome(biome);
    }
    return biome;
}

void BiomeClassifier::adjustBiomeInput(BiomeInput& input) const {
    static constexpr float DRY_LAPSE_RATE = 0.01f;     // 乾燥時每升高 1 公尺降低的溫度
    static constexpr float MOIST_LAPSE_RATE = 0.006f;  // 潮濕時每升高 1 公尺降低的溫度 (水氣凝結放熱，降溫較慢)

    static constexpr float JITTER_SCALE = 16.0f;
    static constexpr float TEMPERATURE_JITTER_STRENGTH = 0.03f;
    static constexpr float HUMIDITY_JITTER_STRENGTH = 0.03f;
    static constexpr float CONTINENT_JITTER_STRENGTH = 0.003f;

    float x = input.worldX / JITTER_SCALE;
    float z = input.worldZ / JITTER_SCALE;
    input.humidity += _humidityJitter.noise(x, z) * HUMIDITY_JITTER_STRENGTH;
    input.temperature += _temperatureJitter.noise(x, z) * TEMPERATURE_JITTER_STRENGTH;

    float lapseRate = lerp(DRY_LAPSE_RATE, MOIST_LAPSE_RATE, input.humidity);
    input.temperature -= lapseRate * std::max(0.0f, input.height);

    if (input.continent >= 0.0f) {
        float continent = input.continent + _continentJitter.noise(x, z) * CONTINENT_JITTER_STRENGTH;
        input.continent = std::max(0.0f, continent);
    }
}

Biome BiomeClassifier::nearestClimateBiome(float temperature, float humidity) const {
    constexpr float TEMPERATURE_WEIGHT = 2.0f;
    constexpr float HUMIDITY_WEIGHT = 1.0f;

    Biome nearestBiome = Biome::Plains;
    float nearestDistance = std::numeric_limits<float>::max();

    for (const auto& point : _biomeClimatePoints) {
        float dt = temperature - point.temperature;
        float dh = humidity - point.humidity;
        float distance = TEMPERATURE_WEIGHT * dt * dt + HUMIDITY_WEIGHT * dh * dh;

        if (distance < nearestDistance) {
            nearestDistance = distance;
            nearestBiome = point.biome;
        }
    }

    return nearestBiome;
}

}  // namespace robot::world::chunk
