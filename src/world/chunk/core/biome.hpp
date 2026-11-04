#pragma once

#include <array>
#include <cstddef>
#include <string>

#include "math/perlin.hpp"
#include "math/random.hpp"

namespace robot::world::chunk {

enum class Biome {

    Ocean,
    FrozenOcean,
    Beach,
    StoneShore,
    Swamp,

    AlpineMeadow,
    StonyPeaks,
    SnowyPeaks,

    // 以下生態系是根據溫度、濕度來分類的

    Plains,
    SnowyPlains,

    Savanna,
    Desert,

    Forest,
    Taiga,
    SnowyTaiga,
    Jungle,

    Count
};

std::string toString(Biome biome);

struct BiomeInput {
    float worldX;
    float worldZ;
    float height;      ///< 地表高度
    float waterLevel;  ///< 水位高度
    float slope;       ///< 坡度
    float continent;
    float temperature;
    float humidity;
    float cliffMask;  ///< 斷崖遮罩
    float hillMask;
    float mountainMask;
    float ridge;  ///< 山脊遮罩
};

class BiomeClassifier {
   public:
    explicit BiomeClassifier(Seed seed);
    ~BiomeClassifier() = default;

    Biome classify(const BiomeInput& input) const;

   private:
    struct ClimatePoints {
        float temperature;
        float humidity;
        Biome biome;
    };

    Perlin2D _temperatureJitter;
    Perlin2D _humidityJitter;
    Perlin2D _continentJitter;

    using ClimatePointArray = std::array<ClimatePoints, static_cast<size_t>(Biome::Count) - 8>;
    static constexpr ClimatePointArray _biomeClimatePoints = {
        {{0.545, 0.354, Biome::Plains},
         {0.145, 0.274, Biome::SnowyPlains},
         {0.818, 0.548, Biome::Savanna},
         {0.818, 0.224, Biome::Desert},
         {0.582, 0.612, Biome::Forest},
         {0.364, 0.500, Biome::Taiga},
         {0.236, 0.418, Biome::SnowyTaiga},
         {0.836, 0.908, Biome::Jungle}}
    };

    /// 根據高度、濕度、隨機抖動等因素調整 BiomeInput 的值，使其更適合用於生物群落分類。
    void adjustBiomeInput(BiomeInput& input) const;
    /// 找到最接近指定溫度和濕度的生態系
    Biome nearestClimateBiome(float temperature, float humidity) const;
};

}  // namespace robot::world::chunk
