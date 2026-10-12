#include "view/entity/creeper_renderer.hpp"

#include <cmath>

#include "common/color.hpp"
#include "graphics/mesh_shape.hpp"
#include "math/constants.hpp"
#include "math/vec3.hpp"
#include "platform/glut.hpp"

namespace robot::view::entity {

namespace {

#pragma region Dimensions

// 尺寸以像素設計，1 px = 1/16 公尺。只定義零件大小，關節位置都由大小推算，改一處就全部跟著變
const float PX = 1.0f / 16.0f;

const Vec3 HEAD_SIZE(8.0f, 8.0f, 8.0f);
const Vec3 BODY_SIZE(8.0f, 12.0f, 4.0f);
const Vec3 LEG_SIZE(4.0f, 6.0f, 4.0f);

const float HIP_Y = LEG_SIZE.y;            // 腿站在地上，髖在腿頂
const float NECK_Y = HIP_Y + BODY_SIZE.y;  // 頭在身體頂端

// 腿凸出在身體前後，只以一條邊和身體底面相接。髖設在這條邊上，擺動時腿才不會離開身體。
// 面向 +z，左手邊為 +x
struct Leg {
    float x;
    float hingeZ;   // 身體的前緣或後緣
    float offsetZ;  // 從髖到腿的中心
};
const float LEG_X = LEG_SIZE.x / 2.0f;
const float LEG_HINGE_Z = BODY_SIZE.z / 2.0f;
const float LEG_OFFSET_Z = LEG_SIZE.z / 2.0f;
const Leg LEGS[] = {
    {LEG_X,  LEG_HINGE_Z,  LEG_OFFSET_Z }, // 左前
    {-LEG_X, LEG_HINGE_Z,  LEG_OFFSET_Z }, // 右前
    {LEG_X,  -LEG_HINGE_Z, -LEG_OFFSET_Z}, // 左後
    {-LEG_X, -LEG_HINGE_Z, -LEG_OFFSET_Z}, // 右後
};

#pragma endregion  // Dimensions

#pragma region Face

// 從正面看苦力怕，X 為黑色；一格一像素，大小與頭的正面相同
const int FACE_SIZE = 8;
// clang-format off
const char* const FACE[FACE_SIZE] = {
    "........",
    ".XX..XX.",
    ".XX..XX.",
    "...XX...",
    "..XXXX..",
    "..XXXX..",
    "..X..X..",
    "........",
};
// clang-format on
const float FACE_DEPTH = 0.1f;  // 凸出頭的正面，避免兩者重疊而閃爍

#pragma endregion  // Face

#pragma region Animation

const float STRIDE_LENGTH = 1.0f;  // 公尺，走這麼遠腿擺一個來回
const float MAX_SWING = 30.0f;     // 度

#pragma endregion  // Animation

const ColorRGBA CREEPER_GREEN(0.35f, 0.75f, 0.30f);

// 臉貼在頭的 +z 面上；第 row 列在最上面，y 因此要反過來算
graphics::Mesh buildFace() {
    graphics::Mesh face;
    const float z = HEAD_SIZE.z / 2.0f;
    for (int row = 0; row < FACE_SIZE; row++) {
        for (int col = 0; col < FACE_SIZE; col++) {
            if (FACE[row][col] != 'X') {
                continue;
            }
            const float x = col - FACE_SIZE / 2.0f;
            const float y = FACE_SIZE - 1.0f - row;
            graphics::addCuboid(face, Vec3(x, y, z) * PX, Vec3(x + 1.0f, y + 1.0f, z + FACE_DEPTH) * PX);
        }
    }
    return face;
}

// 相位跟著走過的距離，停下時不再前進；擺幅跟著 walkAnimation 慢慢收回
float legSwing(const world::entity::Creeper& creeper) {
    const float phase = std::sin(2.0f * PI * creeper.walkDistance() / STRIDE_LENGTH);
    return MAX_SWING * phase * creeper.walkAnimation();
}

}  // namespace

CreeperRenderer::CreeperRenderer()
    : _body(graphics::createCuboid(Vec3(-BODY_SIZE.x / 2.0f, HIP_Y, -BODY_SIZE.z / 2.0f) * PX,
                                   Vec3(BODY_SIZE.x / 2.0f, NECK_Y, BODY_SIZE.z / 2.0f) * PX)),
      _head(graphics::createCuboid(Vec3(-HEAD_SIZE.x / 2.0f, 0.0f, -HEAD_SIZE.z / 2.0f) * PX,
                                   Vec3(HEAD_SIZE.x / 2.0f, HEAD_SIZE.y, HEAD_SIZE.z / 2.0f) * PX)),
      _face(buildFace()),
      _leg(graphics::createCuboid(Vec3(-LEG_SIZE.x / 2.0f, -LEG_SIZE.y, -LEG_SIZE.z / 2.0f) * PX,
                                  Vec3(LEG_SIZE.x / 2.0f, 0.0f, LEG_SIZE.z / 2.0f) * PX)) {}

void CreeperRenderer::render(const world::entity::Creeper& creeper) const {
    glPushAttrib(GL_CURRENT_BIT);  // draw 會改掉目前的顏色
    glPushMatrix();

    const Vec3& position = creeper.position();
    glTranslatef(position.x, position.y, position.z);
    glRotatef(creeper.yaw(), 0.0f, 1.0f, 0.0f);

    _body.draw(CREEPER_GREEN);
    drawHead(creeper.pitch());
    drawLegs(legSwing(creeper));

    glPopMatrix();
    glPopAttrib();
}

void CreeperRenderer::drawHead(float pitch) const {
    glPushMatrix();
    glTranslatef(0.0f, NECK_Y * PX, 0.0f);
    glRotatef(pitch, -1.0f, 0.0f, 0.0f);  // 臉在 +z，繞 -x 正轉才是抬頭
    _head.draw(CREEPER_GREEN);
    _face.draw(Color::Black);
    glPopMatrix();
}

void CreeperRenderer::drawLegs(float swing) const {
    for (const Leg& leg : LEGS) {
        glPushMatrix();
        glTranslatef(leg.x * PX, HIP_Y * PX, leg.hingeZ * PX);
        // x、z 同號的是左前與右後，與另一組對角反向擺動
        glRotatef(leg.x * leg.hingeZ > 0.0f ? -swing : swing, 1.0f, 0.0f, 0.0f);
        glTranslatef(0.0f, 0.0f, leg.offsetZ * PX);
        _leg.draw(CREEPER_GREEN);
        glPopMatrix();
    }
}

}  // namespace robot::view::entity
