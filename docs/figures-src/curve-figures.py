"""src/geometry/curve/README.md 的示意圖。

輸出 docs/figures/curve-*.png。執行：python3 docs/figures-src/curve-figures.py
"""

from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import Polygon

FIGURES = Path(__file__).resolve().parent.parent / "figures"

plt.rcParams.update({
    "font.family": "Noto Sans CJK TC",
    "mathtext.fontset": "cm",
    "font.size": 12,
})

RED = "#b22222"
PURPLE = "#7b3fb5"
GREEN = "#2e7d32"
BLUE = "#1f4fb4"
ORANGE = "#d97706"
GRAY = "#888888"
LIGHT = "#c9d3f5"


def new_axes(width=6.0, height=5.0):
    fig, ax = plt.subplots(figsize=(width, height))
    ax.set_aspect("equal")
    ax.axis("off")
    return fig, ax


def arrow(ax, start, end, color, width=1.8):
    ax.annotate("", xy=end, xytext=start, annotation_clip=False,
                arrowprops=dict(arrowstyle="-|>", color=color, lw=width, mutation_scale=14, shrinkA=0, shrinkB=0))


def dot(ax, p, color="black", size=4):
    ax.plot(*p, "o", color=color, ms=size, zorder=5)


def save(fig, name):
    fig.savefig(FIGURES / f"{name}.png", dpi=200, bbox_inches="tight", facecolor="white")
    plt.close(fig)
    print(f"saved {name}.png")


def arc_points(center, radius, start, sweep, n=100):
    a = np.linspace(start, start + sweep, n)
    return np.column_stack([center[0] + radius * np.cos(a), center[1] + radius * np.sin(a)])


def lerp(a, b, t):
    return a + (b - a) * t


# 1. 圓弧的參數、切線，以及弓形高
def figure_arc():
    fig, (left, right) = plt.subplots(1, 2, figsize=(11, 4.8))
    for ax in (left, right):
        ax.set_aspect("equal")
        ax.axis("off")

    o, r = np.array([0.0, 0.0]), 2.0
    theta0, sweep, t = np.radians(15), np.radians(120), 0.55
    theta = theta0 + sweep * t

    ax = left
    ax.plot(*arc_points(o, r, 0, 2 * np.pi).T, color=LIGHT, lw=1)
    ax.plot(*arc_points(o, r, theta0, sweep).T, color=BLUE, lw=2.5)
    dot(ax, o)
    ax.text(*(o + [-0.15, -0.2]), r"$O$", ha="right")
    ax.plot([0, 2.4], [0, 0], color=GRAY, ls="--", lw=1)
    start, point = arc_points(o, r, theta0, 0, 1)[0], arc_points(o, r, theta, 0, 1)[0]
    ax.plot(*np.array([o, start]).T, color=GRAY, lw=1)
    ax.plot(*np.array([o, point]).T, color=GRAY, lw=1)
    ax.text(*(o + (start - o) * 0.75 + [-0.05, 0.12]), r"$r$")
    a = arc_points(o, 0.55, 0, theta0, 20)
    ax.plot(*a.T, color="black", lw=1)
    ax.text(0.7, 0.07, r"$\theta_0$")
    a = arc_points(o, 0.9, theta0, theta - theta0, 30)
    ax.plot(*a.T, color="black", lw=1)
    ax.text(*(arc_points(o, 1.15, (theta0 + theta) / 2, 0, 1)[0]), r"$\Delta\theta\,t$", ha="center")
    dot(ax, start)
    ax.text(*(start + [0.12, 0]), r"$\gamma(0)$", va="center")
    end = arc_points(o, r, theta0 + sweep, 0, 1)[0]
    dot(ax, end)
    ax.text(*(end + [-0.12, 0.05]), r"$\gamma(1)$", ha="right", va="center")
    dot(ax, point, RED)
    ax.text(*(point + [0.1, 0.12]), r"$\gamma(t)$", color=RED)
    tangent = np.array([-np.sin(theta), np.cos(theta)])
    arrow(ax, point, point + tangent * 1.1, RED)
    ax.text(*(point + tangent * 1.2), r"$[-\sin\theta, \cos\theta]^{\top}$", color=RED, ha="right", va="bottom")
    ax.set_title("參數與切線", fontsize=13)
    ax.set_xlim(-2.8, 2.8)
    ax.set_ylim(-0.6, 2.8)

    ax = right
    phi = np.radians(100)
    a0 = np.pi / 2 - phi / 2
    arc = arc_points(o, r, a0, phi)
    ax.fill(*np.vstack([arc, arc[:1]]).T, color=LIGHT, alpha=0.5)
    ax.plot(*arc.T, color=BLUE, lw=2.5)
    p, q = arc[0], arc[-1]
    ax.plot(*np.array([p, q]).T, color=GREEN, lw=2)
    ax.text(*((p + q) / 2 + [0.6, -0.25]), "弦", color=GREEN)
    mid_chord, mid_arc = (p + q) / 2, np.array([0, r])
    ax.annotate("", xy=mid_arc, xytext=mid_chord,
                arrowprops=dict(arrowstyle="<->", color=RED, lw=1.5, shrinkA=0, shrinkB=0))
    ax.text(*(mid_chord + [0.08, 0.15]), r"$r\,(1 - \cos\frac{\phi}{2})$", color=RED)
    for e in (p, q):
        ax.plot(*np.array([o, e]).T, color=GRAY, lw=1)
    ax.plot(*np.array([o, mid_chord]).T, color=GRAY, lw=1, ls="--")
    ax.plot(*arc_points(o, 0.5, a0, phi, 30).T, color="black", lw=1)
    ax.text(0, 0.62, r"$\phi$", ha="center")
    dot(ax, o)
    ax.text(*(o + [0.1, -0.2]), r"$O$")
    ax.text(*(mid_chord + [-0.15, -0.45]), r"$r\cos\frac{\phi}{2}$", ha="right", color=GRAY)
    ax.set_title("弓形高：弦與弧的最大距離", fontsize=13)
    ax.set_xlim(-2.2, 2.2)
    ax.set_ylim(-0.5, 2.5)

    save(fig, "curve-arc")


# 2. through：兩條中垂線的交點是圓心
def figure_through():
    fig, ax = new_axes()
    a, b, c = np.array([2.0, 0.3]), np.array([0.6, 1.8]), np.array([-1.6, 1.1])
    bb, cc = b - a, c - a
    cross = bb[0] * cc[1] - bb[1] * cc[0]
    u = np.array([cc[1] * bb @ bb - bb[1] * cc @ cc, bb[0] * cc @ cc - cc[0] * bb @ bb]) / (2 * cross)
    o = a + u
    r = np.linalg.norm(a - o)
    ax.plot(*arc_points(o, r, 0, 2 * np.pi).T, color=LIGHT, lw=1)
    angles = [np.arctan2(*(p - o)[::-1]) for p in (a, c)]
    ax.plot(*arc_points(o, r, angles[0], (angles[1] - angles[0]) % (2 * np.pi)).T, color=BLUE, lw=2.5)
    for p, q, color in ((a, b, ORANGE), (a, c, PURPLE)):
        mid, d = (p + q) / 2, q - p
        perp = np.array([-d[1], d[0]]) / np.linalg.norm(d)
        line = np.array([mid - perp * 2.2, mid + perp * 2.0])
        ax.plot(*line.T, color=color, lw=1.2, ls="--")
        ax.plot(*np.array([p, q]).T, color=color, lw=1)
        dot(ax, mid, color, 3)
    dot(ax, o, RED, 5)
    ax.text(*(o + [0.15, -0.05]), "圓心", color=RED, va="center")
    for p, name, offset in ((a, r"$A$（start）", [0.1, -0.25]), (b, r"$B$（via）", [0.05, 0.15]),
                            (c, r"$C$（end）", [-0.1, 0.12])):
        dot(ax, p)
        ax.text(*(p + offset), name, ha="right" if p is c else "left")
    ax.text(1.7, 1.6, r"$AB$ 的中垂線" "\n" r"$2\,u \cdot b = \Vert b \Vert^2$", color=ORANGE, fontsize=11)
    ax.text(-2.3, -0.9, r"$AC$ 的中垂線" "\n" r"$2\,u \cdot c = \Vert c \Vert^2$", color=PURPLE, fontsize=11)
    ax.set_xlim(-2.5, 3.2)
    ax.set_ylim(-1.7, 2.6)
    save(fig, "curve-arc-through")


# 3. tangentAtStart：圓心在垂直於起始方向的直線上
def figure_tangent():
    fig, ax = new_axes()
    a, d, e = np.array([0.0, 0.0]), np.array([1.0, 0.25]), np.array([2.2, 1.6])
    d /= np.linalg.norm(d)
    m = np.array([-d[1], d[0]])
    w = e - a
    s = (w @ w) / (2 * (m @ w))
    o = a + s * m
    r = abs(s)
    ax.plot(*arc_points(o, r, 0, 2 * np.pi).T, color=LIGHT, lw=1)
    a0 = np.arctan2(*(a - o)[::-1])
    a1 = np.arctan2(*(e - o)[::-1])
    ax.plot(*arc_points(o, r, a0, (a1 - a0) % (2 * np.pi)).T, color=BLUE, lw=2.5)
    ax.plot(*np.array([a - d * 1.2, a + d * 3.2]).T, color=GRAY, lw=1, ls="--")
    ax.text(*(a + d * 3.25), "切線", color=GRAY, va="center")
    ax.plot(*np.array([a - m * 0.6, o + m * 0.6]).T, color=ORANGE, lw=1.2, ls="--")
    ax.text(*(a + m * (s * 0.55) + [-0.15, 0]), "圓心在這條\n垂直線上", color=ORANGE, ha="right", va="center", fontsize=11)
    arrow(ax, a, a + d * 1.0, RED)
    ax.text(*(a + d * 1.0 + [0.0, -0.25]), r"$d$", color=RED)
    arrow(ax, a, a + m * 0.8, ORANGE, 1.4)
    ax.text(*(a + m * 0.8 + [-0.15, 0]), r"$\hat m$", color=ORANGE, ha="right")
    ax.plot(*np.array([e, o]).T, color=GRAY, lw=1)
    ax.text(*(a + m * (s * 0.8) + [0.12, 0]), r"$s$", color=ORANGE)
    ax.plot(*np.array([a, e]).T, color=PURPLE, lw=1)
    ax.text(*((a + e) / 2 + [-0.3, 0.15]), r"$w = E - A$", color=PURPLE, ha="right")
    dot(ax, o, RED, 5)
    ax.text(*(o + [0.12, 0.05]), r"圓心 $A + s\,\hat m$", color=RED)
    dot(ax, a)
    ax.text(*(a + [-0.12, -0.25]), r"$A$（start）", ha="right")
    dot(ax, e)
    ax.text(*(e + [0.1, 0]), r"$E$（end）", va="center")
    ax.set_xlim(-1.8, 4.0)
    ax.set_ylim(-1.0, 3.6)
    save(fig, "curve-arc-tangent")


# 4. de Casteljau 與 split
def figure_casteljau():
    fig, ax = new_axes(7, 4.6)
    p = np.array([[0.0, 0.0], [0.8, 2.2], [3.0, 2.4], [3.8, 0.4]])
    t = 0.4
    p01, p12, p23 = lerp(p[0], p[1], t), lerp(p[1], p[2], t), lerp(p[2], p[3], t)
    q0, q1 = lerp(p01, p12, t), lerp(p12, p23, t)
    g = lerp(q0, q1, t)
    ts = np.linspace(0, 1, 200)[:, None]
    curve = ((1 - ts) ** 3 * p[0] + 3 * (1 - ts) ** 2 * ts * p[1] + 3 * (1 - ts) * ts ** 2 * p[2] + ts ** 3 * p[3])
    split = int(t * 199)
    ax.plot(*curve[:split + 1].T, color=PURPLE, lw=3, alpha=0.8, label="前半段")
    ax.plot(*curve[split:].T, color=GREEN, lw=3, alpha=0.8, label="後半段")
    ax.plot(*p.T, color=GRAY, lw=1, ls="--")
    ax.plot(*np.array([p01, p12, p23]).T, color=ORANGE, lw=1)
    ax.plot(*np.array([q0, q1]).T, color=RED, lw=1.5)
    for pt, name, off in ((p[0], "$P_0$", [-0.1, -0.2]), (p[1], "$P_1$", [-0.15, 0.08]), (p[2], "$P_2$", [0.05, 0.1]),
                          (p[3], "$P_3$", [0.1, -0.1])):
        dot(ax, pt, GRAY)
        ax.text(*(pt + off), name, ha="right" if off[0] < 0 else "left")
    for pt, name, off in ((p01, r"$p_{01}$", [-0.12, 0]), (p12, r"$p_{12}$", [0, 0.12]), (p23, r"$p_{23}$", [0.12, 0])):
        dot(ax, pt, ORANGE)
        ax.text(*(pt + off), name, color=ORANGE, ha="right" if off[0] < 0 else ("center" if off[0] == 0 else "left"),
                va="bottom" if off[1] > 0 else "center")
    for pt, name, off in ((q0, "$q_0$", [-0.12, 0.05]), (q1, "$q_1$", [0.1, 0.05])):
        dot(ax, pt, RED)
        ax.text(*(pt + off), name, color=RED, ha="right" if off[0] < 0 else "left")
    dot(ax, g, "black", 6)
    ax.text(*(g + [0.05, -0.28]), r"$\gamma(t)$", ha="center")
    ax.text(0.2, 2.6, r"$t = 0.4$", fontsize=12)
    ax.text(3.6, 2.3, r"$q_0 q_1$ 與曲線相切" "\n" r"$\gamma'(t) = 3\,(q_1 - q_0)$", color=RED, fontsize=11)
    ax.text(-0.3, -0.75, r"前半段：$P_0,\ p_{01},\ q_0,\ \gamma(t)$", color=PURPLE)
    ax.text(2.3, -0.75, r"後半段：$\gamma(t),\ q_1,\ p_{23},\ P_3$", color=GREEN)
    ax.set_xlim(-0.6, 5.6)
    ax.set_ylim(-1.0, 2.9)
    save(fig, "curve-bezier-casteljau")


def point_to_segment(q, a, b):
    ab = b - a
    t = np.clip((q - a) @ ab / (ab @ ab), 0, 1)
    return a + ab * t


# 5. 判斷平坦：凸包與控制點到弦的距離
def figure_flat():
    fig, ax = new_axes(7, 3.8)
    p = np.array([[0.0, 0.0], [1.2, 1.1], [2.8, -0.6], [4.0, 0.2]])
    ts = np.linspace(0, 1, 200)[:, None]
    curve = ((1 - ts) ** 3 * p[0] + 3 * (1 - ts) ** 2 * ts * p[1] + 3 * (1 - ts) * ts ** 2 * p[2] + ts ** 3 * p[3])
    ax.add_patch(Polygon(p[[0, 1, 3, 2]], closed=True, color=LIGHT, alpha=0.6, lw=0))
    ax.text(1.4, -0.35, "凸包", color=BLUE, fontsize=11)
    ax.plot(*curve.T, color=BLUE, lw=2.5)
    ax.plot(*p[[0, 3]].T, color=GREEN, lw=2)
    ax.text(3.3, 0.3, "弦", color=GREEN)
    for i, name in ((1, "$P_1$"), (2, "$P_2$")):
        foot = point_to_segment(p[i], p[0], p[3])
        ax.annotate("", xy=foot, xytext=p[i], arrowprops=dict(arrowstyle="<->", color=RED, lw=1.4, shrinkA=0, shrinkB=0))
        ax.text(*((p[i] + foot) / 2 + [0.1, 0]), f"$d_{i}$", color=RED, va="center")
        dot(ax, p[i], GRAY)
        ax.text(*(p[i] + [0.1, 0.08 if i == 1 else -0.2]), name)
    for i, name, off in ((0, "$P_0$", [-0.1, -0.2]), (3, "$P_3$", [0.1, -0.05])):
        dot(ax, p[i])
        ax.text(*(p[i] + off), name, ha="right" if off[0] < 0 else "left")
    ax.text(-0.2, -1.05, r"$d_1 \leq \delta$ 且 $d_2 \leq \delta$ 時，凸包內每一點、也就是整條曲線，到弦的距離都不超過 $\delta$",
            fontsize=11)
    ax.set_xlim(-0.4, 4.6)
    ax.set_ylim(-1.2, 1.4)
    save(fig, "curve-bezier-flat")


def flatten_cubic(p, tol, depth=0, out=None):
    if out is None:
        out = [p[0]]
    flat = all(np.linalg.norm(p[i] - point_to_segment(p[i], p[0], p[3])) <= tol for i in (1, 2))
    if flat or depth >= 10:
        out.append(p[3])
        return out
    p01, p12, p23 = lerp(p[0], p[1], 0.5), lerp(p[1], p[2], 0.5), lerp(p[2], p[3], 0.5)
    q0, q1 = lerp(p01, p12, 0.5), lerp(p12, p23, 0.5)
    g = lerp(q0, q1, 0.5)
    flatten_cubic(np.array([p[0], p01, q0, g]), tol, depth + 1, out)
    flatten_cubic(np.array([g, q1, p23, p[3]]), tol, depth + 1, out)
    return out


# 6. flatten：彎曲處切得較細
def figure_flatten():
    fig, axes = plt.subplots(1, 2, figsize=(11, 3.8))
    p = np.array([[0.0, 0.0], [3.2, 0.0], [4.0, 0.2], [4.0, 2.2]])
    ts = np.linspace(0, 1, 300)[:, None]
    curve = ((1 - ts) ** 3 * p[0] + 3 * (1 - ts) ** 2 * ts * p[1] + 3 * (1 - ts) * ts ** 2 * p[2] + ts ** 3 * p[3])
    for ax, tol in zip(axes, (0.08, 0.01)):
        ax.set_aspect("equal")
        ax.axis("off")
        pts = np.array(flatten_cubic(p, tol))
        ax.plot(*curve.T, color=LIGHT, lw=5)
        ax.plot(*pts.T, color=BLUE, lw=1.2)
        ax.plot(*pts.T, "o", color=RED, ms=3.5)
        ax.set_title(rf"$\delta = {tol}$：{len(pts) - 1} 段，彎曲處較密", fontsize=13)
    save(fig, "curve-flatten")


# 7. Path2D：剖面、法向量與折角
def figure_path2d():
    fig, axes = plt.subplots(1, 2, figsize=(9, 5))
    r, h = 1.0, 2.0

    ax = axes[0]
    pts = [((0, 0), (0, -1)), ((r, 0), (0, -1)), ((r, 0), (1, 0)), ((r, h), (1, 0)), ((r, h), (0, 1)), ((0, h), (0, 1))]
    ax.plot([0, r, r, 0], [0, 0, h, h], color=BLUE, lw=2.5)
    for (x, y), (nx, ny) in pts:
        arrow(ax, (x, y), (x + nx * 0.45, y + ny * 0.45), RED, 1.4)
    for corner in ((r, 0), (r, h)):
        ax.add_patch(plt.Circle(corner, 0.12, fill=False, color=ORANGE, lw=1.5))
    ax.text(r + 0.25, -0.75, "折角：同一點兩次，\n各帶自己的法向量", color=ORANGE, fontsize=10)
    ax.set_title("柱", fontsize=13)

    ax = axes[1]
    ang1 = np.linspace(-np.pi / 2, 0, 9)
    ang2 = np.linspace(0, np.pi / 2, 9)
    lower = np.column_stack([r * np.cos(ang1), r * np.sin(ang1)])
    side = np.array([[r, 0.0], [r, h]])
    upper = np.column_stack([r * np.cos(ang2), h + r * np.sin(ang2)])
    profile = np.vstack([lower, side[1:], upper[1:]])
    ax.plot(*profile.T, color=BLUE, lw=2.5)
    normals = [(pt, pt / np.linalg.norm(pt)) for pt in lower] + [((r, h / 2), np.array([1.0, 0]))]
    normals += [(pt, (pt - [0, h]) / np.linalg.norm(pt - [0, h])) for pt in upper]
    for pt, n in normals:
        arrow(ax, pt, pt + n * 0.4, RED, 1.2)
    ax.text(r + 0.55, h / 2 + 0.5, "接點都平順，\n沒有折角", color=GREEN, fontsize=10, va="center")
    ax.set_title("膠囊", fontsize=13)

    for ax in axes:
        ax.set_aspect("equal")
        ax.axhline(0, color=GRAY, lw=0.5, ls=":")
        ax.axvline(0, color="black", lw=1)
        ax.text(0.05, h + r + 0.55, "旋轉軸（x = 0）", fontsize=10)
        arrow(ax, (-0.35, -0.6), (-0.35, h + 0.3), GRAY, 1)
        ax.text(-0.45, 0.6, "由下往上畫", color=GRAY, rotation=90, fontsize=10, ha="right")
        ax.set_xlim(-0.8, 2.6)
        ax.set_ylim(-1.5, h + r + 0.8)
        ax.axis("off")
    save(fig, "curve-path2d")


if __name__ == "__main__":
    FIGURES.mkdir(exist_ok=True)
    figure_arc()
    figure_through()
    figure_tangent()
    figure_casteljau()
    figure_flat()
    figure_flatten()
    figure_path2d()
