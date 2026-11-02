#!/usr/bin/env bash
set -euo pipefail

# 以截圖模式執行遊戲，把截到的 BMP 轉成 PNG 並裁切後放到 docs/figures/。
# 需要先建置 build/main；轉檔與裁切使用 macOS 內建的 sips。

script_dir="$(cd "$(dirname "$0")" && pwd)"
root_dir="$(cd "$script_dir/../.." && pwd)"
figure_dir="$root_dir/docs/figures"
raw_dir="$(mktemp -d)"
trap 'rm -r "$raw_dir"' EXIT

ROBOT_SCREENSHOT_DIR="$raw_dir" "$root_dir/build/main"

# 遊玩畫面保留整個視窗與 HUD
sips -s format png "$raw_dir/gameplay.bmp" --out "$figure_dir/gameplay.png" >/dev/null

# 機器人特寫：機器人的眼睛在畫面中央，往下裁一塊正方形包住全身
for name in robot-front robot-side robot-back stance-standing stance-sneaking stance-sitting; do
    sips -s format png -c 360 360 --cropOffset 255 300 "$raw_dir/$name.bmp" --out "$figure_dir/$name.png" >/dev/null
done

# 地形與障礙物：裁掉左上角的 HUD
for name in obstacles terrain-overview; do
    sips -s format png -c 600 800 --cropOffset 110 160 "$raw_dir/$name.bmp" --out "$figure_dir/$name.png" >/dev/null
done

echo "Screenshots saved to $figure_dir"
