"""繞任意軸 n 旋轉 θ：v 拆成沿軸與垂直於軸的分量，垂直分量在垂直於軸的平面上轉 θ。

輸出 docs/figures/rotation-axis-angle.png。執行：python3 docs/figures-src/rotation-axis-angle.py
"""

from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import MultipleLocator
from mpl_toolkits.mplot3d.art3d import Poly3DCollection

OUTPUT = Path(__file__).resolve().parent.parent / "figures" / "rotation-axis-angle.png"

plt.rcParams.update({
    "font.family": "Noto Sans CJK TC",
    "mathtext.fontset": "cm",
    "font.size": 12,
})

RED = "#b22222"
PURPLE = "#7b3fb5"
GREEN = "#2e7d32"
TEAL = "#00838f"
BLUE = "#1f4fb4"
ORANGE = "#d97706"
GRAY = "#777777"

# 任意方向的單位軸 n：從 +z 往 -y 傾斜 15°
tilt = np.radians(15)
n = np.array([0.0, -np.sin(tilt), np.cos(tilt)])

# 垂直於 n 的平面上取正交的 a、b，v⊥ 在其中的方向決定 v；數值是為了讓各向量在畫面上分得開
a = np.cross(n, [0.0, 0.0, 1.0])
a /= np.linalg.norm(a)
b = np.cross(n, a)
phi = np.radians(200)
height, radius = 2.2, 1.6                 # n · v、|v⊥|
v = height * n + radius * (np.cos(phi) * a + np.sin(phi) * b)
theta = np.radians(55)

parallel = np.dot(n, v) * n              # (n · v) n，旋轉時不變
perp = v - parallel                       # v⊥
side = np.cross(n, v)                     # n × v，v⊥ 繞軸轉 90°
rv = parallel + np.cos(theta) * perp + np.sin(theta) * side

# 垂直分量所在平面上的單位方向，用來畫圓與角度
u = perp / radius
w = side / radius
origin = np.zeros(3)


def on_circle(angle, scale=1.0):
    return parallel + scale * radius * (np.cos(angle) * u + np.sin(angle) * w)


fig = plt.figure(figsize=(7.5, 7.5))
ax = fig.add_subplot(projection="3d")


def arrow(start, end, color, width=2.0):
    d = end - start
    ax.quiver(*start, *d, color=color, linewidth=width, arrow_length_ratio=0.25 / np.linalg.norm(d))


def label(point, text, color="black", **kwargs):
    ax.text(*point, text, color=color, **kwargs)


# 垂直於軸的平面：v⊥ 旋轉時的軌跡
circle = np.array([on_circle(a) for a in np.linspace(0, 2 * np.pi, 120)])
ax.add_collection3d(Poly3DCollection([circle], facecolor="#c9d3f5", alpha=0.35, edgecolor="none"))
ax.plot(*circle.T, color="#8796d8", linewidth=1)

# 軸 n
ax.plot(*np.array([-0.4 * n, origin]).T, color=GRAY, linestyle="--", linewidth=1)
ax.plot(*np.array([origin, parallel]).T, color="black", linewidth=1)
arrow(parallel, 3.8 * n, "black", width=2.2)
label(4.0 * n, r"$n$（單位軸）", ha="center")

# v 與 Rv
arrow(origin, v, RED)
label(0.55 * v + np.array([0.15, 0.1, -0.2]), r"$v$", RED, fontsize=14)
arrow(origin, rv, GREEN)
label(rv + np.array([0.05, 0.05, 0.1]), r"$Rv$", GREEN, fontsize=14)

# 沿軸的分量
arrow(origin, parallel, ORANGE, width=1.8)
label(0.5 * parallel + np.array([-0.15, 0.0, 0.0]), r"$(n \cdot v)\,n$", ORANGE, ha="right")

# 垂直分量 v⊥、n × v、轉 θ 後的分量
arrow(parallel, v, PURPLE, width=1.8)
label(v + np.array([0.08, 0.0, 0.08]), r"$v_\perp$", PURPLE, fontsize=14)
arrow(parallel, parallel + side, BLUE, width=1.8)
label(parallel + side + np.array([0.15, 0.0, 0.1]), r"$n \times v$", BLUE, ha="left")
arrow(parallel, rv, TEAL, width=1.8)
label(parallel + 0.45 * (rv - parallel) + np.array([0.0, 0.0, 0.1]), r"$Rv_\perp$", TEAL,
      ha="center", va="bottom")

# cos θ v⊥ + sin θ (n × v) 組成旋轉後的垂直分量
cos_point = parallel + np.cos(theta) * perp
sin_point = parallel + np.sin(theta) * side
for start in (cos_point, sin_point):
    ax.plot(*np.array([start, rv]).T, color=GRAY, linestyle="--", linewidth=1)
ax.scatter(*np.array([cos_point, sin_point]).T, color="black", s=12, depthshade=False)
label(cos_point + np.array([0.0, 0.0, -0.3]), r"$\cos\theta\, v_\perp$", PURPLE, ha="center")
label(sin_point + np.array([-0.1, 0.0, -0.05]), r"$\sin\theta\,(n \times v)$", ha="right", va="top")

# 角 θ
arc = np.array([on_circle(a, 0.35) for a in np.linspace(0, theta, 30)])
ax.plot(*arc.T, color="black", linewidth=1.2)
label(on_circle(theta / 2, 0.5), r"$\theta$", fontsize=14, ha="center", va="center")

ax.scatter(*origin, color="black", s=12, depthshade=False)
label(np.array([0.0, 0.0, -0.25]), r"$O$", ha="right")

# 三軸範圍等長、比例 1:1:1，方向與角度才不會變形
# 以圖形的中心為準，三軸取相同的範圍
extent = np.vstack([circle, [origin, 4.0 * n]])
center = (extent.min(axis=0) + extent.max(axis=0)) / 2
half = (extent.max(axis=0) - extent.min(axis=0)).max() / 2 + 0.3
ax.set_xlim(center[0] - half, center[0] + half)
ax.set_ylim(center[1] - half, center[1] + half)
ax.set_zlim(center[2] - half, center[2] + half)
ax.set_box_aspect((1, 1, 1))
for axis in (ax.xaxis, ax.yaxis, ax.zaxis):
    axis.set_major_locator(MultipleLocator(1.0))
ax.set_xlabel("X")
ax.set_ylabel("Y")
ax.set_zlabel("Z")
ax.view_init(elev=25, azim=-60)

fig.text(0.1, 0.0,
         r"$Rv = (n \cdot v)\,n + \cos\theta\, v_\perp + \sin\theta\,(n \times v)$" "\n"
         r"沿軸的 $(n \cdot v)\,n$ 不變；垂直分量 $v_\perp = v - (n \cdot v)\,n$ 轉 $\theta$，"
         r"$n \times v$ 是 $v_\perp$ 繞軸轉 $90^\circ$",
         ha="left", va="bottom", fontsize=12, linespacing=1.8)

fig.savefig(OUTPUT, dpi=200, bbox_inches="tight", facecolor="white")
print(f"saved {OUTPUT}")
