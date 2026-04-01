#!/usr/bin/env bash
# OO Bootstrap — clone tous les repos de l'écosystème en une commande
# Usage: bash bootstrap.sh [target_dir]

set -e

OO_REPOS=(
    "llm-baremetal:https://github.com/Djiby-diop/llm-baremetal.git"
    "oo-host:https://github.com/Djiby-diop/oo-host.git"
    "oo-sim:https://github.com/Djiby-diop/oo-sim.git"
    "oo-lab:https://github.com/Djiby-diop/oo-lab.git"
    "oo-dplus:https://github.com/Djiby-diop/oo-dplus.git"
)

TARGET="${1:-$HOME/oo-workspace}"

echo "=== OO Bootstrap ==="
echo "Target: $TARGET"
mkdir -p "$TARGET"
cd "$TARGET"

for entry in "${OO_REPOS[@]}"; do
    name="${entry%%:*}"
    url="${entry##*:}"
    if [ -d "$name/.git" ]; then
        echo "[ok]   $name already cloned — pulling latest..."
        git -C "$name" pull --ff-only 2>/dev/null || echo "[warn] $name: pull failed (skip)"
    else
        echo "[clone] $name"
        git clone "$url" "$name"
    fi
done

echo ""
echo "=== OO workspace ready at: $TARGET ==="
echo "Run: cd $TARGET && oo-system/scripts/status.sh"
