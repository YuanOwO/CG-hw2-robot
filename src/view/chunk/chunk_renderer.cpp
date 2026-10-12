#include "view/chunk/chunk_renderer.hpp"

#include "common/color.hpp"
#include "platform/glut.hpp"

namespace robot::view::chunk {

namespace {

const ColorRGBA GRASS_LIGHT{0.56f, 0.76f, 0.40f};
const ColorRGBA GRASS_DARK{0.48f, 0.68f, 0.34f};

// 1x1 的棋盤格，移動時才看得出走了多遠
void drawGround(const world::chunk::Chunk& chunk) {
    using world::chunk::CHUNK_SIZE;

    const int originX = chunk.pos.x * CHUNK_SIZE;
    const int originZ = chunk.pos.z * CHUNK_SIZE;
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    for (int z = originZ; z < originZ + CHUNK_SIZE; z++) {
        for (int x = originX; x < originX + CHUNK_SIZE; x++) {
            // 以世界座標決定顏色，相鄰 chunk 的格子才接得上
            const ColorRGBA& color = (x + z) % 2 == 0 ? GRASS_LIGHT : GRASS_DARK;
            glColor3f(color.r, color.g, color.b);

            // 從上方看為逆時針，正面朝上
            const float x0 = static_cast<float>(x);
            const float z0 = static_cast<float>(z);
            glVertex3f(x0, 0.0f, z0);
            glVertex3f(x0, 0.0f, z0 + 1.0f);
            glVertex3f(x0 + 1.0f, 0.0f, z0 + 1.0f);
            glVertex3f(x0 + 1.0f, 0.0f, z0);
        }
    }
    glEnd();
}

}  // namespace

void drawChunk(const world::chunk::Chunk& chunk) {
    drawGround(chunk);
}

}  // namespace robot::view::chunk
