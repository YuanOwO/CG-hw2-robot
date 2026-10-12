#pragma once

#include "graphics/mesh.hpp"
#include "math/vec3.hpp"

// 產生網格的函式。網格建立一次後可以重複畫，與 primitives 當下直接畫出不同。

namespace robot::graphics {

// 把軸對齊的長方體加進既有的網格。每個面各自有 4 個頂點，法向量才能垂直於該面，邊緣是銳利的而不是被平均成圓的
void addCuboid(Mesh& mesh, const Vec3& min, const Vec3& max);

// 只有一個長方體的網格
Mesh createCuboid(const Vec3& min, const Vec3& max);

}  // namespace robot::graphics
