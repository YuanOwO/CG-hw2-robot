#include "world/entity/robot_spec.hpp"

#include "common/color.hpp"
#include "common/units.hpp"
#include "geometry/curve/path2d.hpp"
#include "geometry/mesh/mesh.hpp"
#include "geometry/mesh/mesh_builder.hpp"
#include "math/vec2.hpp"
#include "math/vec3.hpp"
#include "world/anatomy/joint.hpp"

namespace robot::world::entity::robot_spec {

namespace {

constexpr ColorRGBA shellColor{0.52f, 0.72f, 0.92f};  // 淺藍外殼
constexpr ColorRGBA panelColor{0.22f, 0.54f, 0.87f};  // 深藍頭部、腰帶
constexpr ColorRGBA jointColor{0.05f, 0.27f, 0.49f};  // 關節、脖子、面罩
constexpr ColorRGBA eyeColor{0.90f, 0.95f, 0.98f};    // 眼睛
constexpr ColorRGBA tipColor{0.85f, 0.35f, 0.19f};    // 天線頂端

// 部件只有幾 px 大，預設的容差太粗，曲線會切成明顯的折線
geometry::MeshBuilder makeBuilder() {
    return geometry::MeshBuilder(32, 0.05f * PX);
}

geometry::Mesh robotTorsoMesh() {
    // 上下等寬的圓筒，半徑等於軀幹半寬；剖面分三段上色，交界處都在鉛直的側面上，接縫看不出來
    geometry::Path2D hip(Vec2(0.0f, 0.0f));
    hip.lineTo(Vec2(3.4f, 0.0f) * PX).arcTo(Vec2(3.4f, 0.6f) * PX, Vec2(4.0f, 0.6f) * PX).lineTo(Vec2(4.0f, 1.4f) * PX);

    geometry::Path2D belt(Vec2(4.0f, 1.4f) * PX);
    belt.lineTo(Vec2(4.0f, 2.6f) * PX);

    // 肩膀只在最上方收圓：在肩膀高度 (y = 9) 半徑仍約 3.8，略大於肩膀球的內緣 3.6，手臂才會嵌進軀幹
    geometry::Path2D chest(Vec2(4.0f, 2.6f) * PX);
    chest.lineTo(Vec2(4.0f, 8.4f) * PX)
        .cubicBezierTo(Vec2(4.0f, 9.3f) * PX, Vec2(3.2f, 9.6f) * PX, Vec2(2.0f, 9.6f) * PX)
        .lineTo(Vec2(0.0f, 9.6f) * PX);

    geometry::MeshBuilder builder = makeBuilder();
    builder.transform(scale(1.0f, 1.0f, 0.6f));  // FIXME: 目前不支援非等比例縮放，法向量會錯亂
    builder.setColor(shellColor).lathe(hip).lathe(chest);
    builder.setColor(panelColor).lathe(belt);
    return builder.build();
}

geometry::Mesh robotNeckMesh() {
    geometry::MeshBuilder builder = makeBuilder();
    builder.setColor(jointColor).cylinder(0.9f * PX, 1.0f * PX);
    return builder.build();
}

geometry::Mesh robotHeadMesh() {
    // 平底，側面先微微外鼓，再收成圓頂；終點的切線水平，頂端才不會尖
    geometry::Path2D profile(Vec2(0.0f, 0.0f));
    profile.lineTo(Vec2(2.2f, 0.0f) * PX)
        .cubicBezierTo(Vec2(2.4f, 0.8f) * PX, Vec2(2.5f, 1.8f) * PX, Vec2(2.5f, 2.5f) * PX)
        .cubicBezierTo(Vec2(2.5f, 4.0f) * PX, Vec2(1.4f, 5.0f) * PX, Vec2(0.0f, 5.0f) * PX);

    geometry::MeshBuilder builder = makeBuilder();
    builder.setColor(panelColor).lathe(profile);

    // 面罩比頭稍大一圈，繞頭一周
    builder.setColor(jointColor).transform(translation(Vec3(0.0f, 1.4f, 0.0f) * PX)).cylinder(2.56f * PX, 1.6f * PX);

    // 眼睛一半嵌在面罩裡
    builder.setColor(eyeColor);
    builder.transform(translation(Vec3(0.9f, 2.2f, 2.45f) * PX)).sphere(0.35f * PX);
    builder.transform(translation(Vec3(-0.9f, 2.2f, 2.45f) * PX)).sphere(0.35f * PX);

    // 耳朵是轉成水平的短圓柱，底面嵌進頭裡，從面罩外凸出
    builder.setColor(jointColor);
    builder.transform(translation(Vec3(2.3f, 2.2f, 0.0f) * PX) * rotation(-90.0f, Vec3(0.0f, 0.0f, 1.0f)))
        .cylinder(0.6f * PX, 0.5f * PX);
    builder.transform(translation(Vec3(-2.3f, 2.2f, 0.0f) * PX) * rotation(90.0f, Vec3(0.0f, 0.0f, 1.0f)))
        .cylinder(0.6f * PX, 0.5f * PX);

    // 天線偏左後方，底部埋進圓頂
    builder.transform(translation(Vec3(0.6f, 4.5f, -0.6f) * PX)).cylinder(0.15f * PX, 2.8f * PX);
    builder.setColor(tipColor).transform(translation(Vec3(0.6f, 7.5f, -0.6f) * PX)).sphere(0.4f * PX);

    return builder.build();
}

/// 關節球加上往 -y 垂下的膠囊；膠囊頂端藏在球裡
geometry::Mesh limbMesh(float jointRadius, float limbRadius, float limbTop, float limbBottom) {
    geometry::MeshBuilder builder = makeBuilder();
    builder.setColor(jointColor).sphere(jointRadius * PX);
    builder.setColor(shellColor)
        .transform(translation(Vec3(0.0f, limbBottom, 0.0f) * PX))
        .capsule(limbRadius * PX, (limbTop - limbBottom) * PX);
    return builder.build();
}

geometry::Mesh robotUpperArmMesh() {
    return limbMesh(1.9f, 1.3f, -0.8f, -4.6f);
}

geometry::Mesh robotForearmMesh() {
    return limbMesh(1.15f, 1.4f, -0.4f, -6.0f);
}

geometry::Mesh robotThighMesh() {
    return limbMesh(1.6f, 1.7f, -0.8f, -5.4f);
}

geometry::Mesh robotShinMesh() {
    // 平底站在地上，頂端是半球；半球低於轉軸，膝蓋球從上方露出
    geometry::Path2D profile(Vec2(0.0f, -7.0f) * PX);
    profile.lineTo(Vec2(1.8f, -7.0f) * PX)
        .lineTo(Vec2(1.8f, -2.4f) * PX)
        .arcTo(Vec2(0.0f, -2.4f) * PX, Vec2(0.0f, -0.6f) * PX, geometry::ArcDirection::CounterClockwise);

    geometry::MeshBuilder builder = makeBuilder();
    builder.setColor(jointColor).sphere(1.4f * PX);
    builder.setColor(shellColor).lathe(profile);

    return builder.build();
}

}  // namespace

anatomy::BipedTorsoSpec robotTorsoSpec() {
    return anatomy::BipedTorsoSpec{
        Vec3(8.0f, 9.0f, 4.0f) * PX,                 // size
        anatomy::JointLimits(-45.0f, 45.0f, 90.0f),  // pitch
        anatomy::JointLimits(),                      // roll
        robotTorsoMesh(),                            // torsoMesh
        Vec3(0.0f, 9.0f, 0.0f) * PX,                 // neckOffset
        9.0f * PX,                                   // shoulderHeight
        4.0f * PX,                                   // hipSpacing
    };
}

anatomy::NeckSpec robotNeckSpec() {
    return anatomy::NeckSpec{
        Vec3(2.0f, 1.0f, 2.0f) * PX,                 // size
        anatomy::JointLimits(),                      // yaw
        anatomy::JointLimits(-45.0f, 45.0f, 90.0f),  // pitch
        anatomy::JointLimits(),                      // roll
        robotNeckMesh(),                             // neckMesh
    };
}

anatomy::HeadSpec robotHeadSpec() {
    return anatomy::HeadSpec{
        Vec3(5.0f, 5.0f, 5.0f) * PX,                 // size
        anatomy::JointLimits(-70.0f, 70.0f, 90.0f),  // yaw
        anatomy::JointLimits(-60.0f, 50.0f, 90.0f),  // pitch
        robotHeadMesh(),                             // headMesh
    };
}

anatomy::ArmSpec robotArmSpec() {
    return anatomy::ArmSpec{
        Vec3(3.0f, 5.0f, 3.0f) * PX,                  // upperArmSize
        anatomy::JointLimits(),                       // shoulderYaw
        anatomy::JointLimits(-60.0f, 200.0f, 90.0f),  // shoulderPitch
        anatomy::JointLimits(0.0f, 170.0f, 90.0f),    // shoulderRoll
        robotUpperArmMesh(),                          // upperArmMesh
        Vec3(3.0f, 6.0f, 3.0f) * PX,                  // forearmSize
        anatomy::JointLimits(0.0f, 145.0f, 90.0f),    // elbowPitch
        robotForearmMesh(),                           // forearmMesh
    };
}

anatomy::LegSpec robotLegSpec() {
    return anatomy::LegSpec{
        Vec3(4.0f, 6.0f, 4.0f) * PX,                  // thighSize
        anatomy::JointLimits(0.0f, 45.0f, 90.0f),     // hipYaw
        anatomy::JointLimits(-30.0f, 120.0f, 90.0f),  // hipPitch
        anatomy::JointLimits(),                       // hipRoll
        robotThighMesh(),                             // thighMesh
        Vec3(4.0f, 7.0f, 4.0f) * PX,                  // shinSize
        anatomy::JointLimits(-140.0f, 0.0f, 90.0f),   // kneePitch
        robotShinMesh(),                              // shinMesh
    };
}

}  // namespace robot::world::entity::robot_spec
