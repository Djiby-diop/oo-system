#!/usr/bin/env bash
# OO Status — afficher l'état de tous les repos OO
# Usage: bash status.sh [workspace_dir]

WORKSPACE="${1:-$(dirname "$(dirname "$0")")/..}"
WORKSPACE="$(cd "$WORKSPACE" && pwd)"

REPOS=(
    "llm-baremetal:1-Cognitive Core"
    "oo-host:2-Execution Kernel"
    "oo-sim:3-Simulation"
    "oo-lab:4-Research"
    "oo-dplus:5-Evolution"
    "oo-system:6+7-Meta+Interface"
)

echo "OO Operating Organism — Repo Status"
echo "Workspace: $WORKSPACE"
printf '%-20s %-30s %-20s %s\n' "REPO" "LAYER" "BRANCH" "LAST COMMIT"
printf '%0.s─' {1..90}; echo

for entry in "${REPOS[@]}"; do
    name="${entry%%:*}"
    label="${entry##*:}"
    path="$WORKSPACE/$name"

    if [ ! -d "$path/.git" ]; then
        printf '%-20s %-30s %-20s %s\n' "$name" "$label" "NOT FOUND" "—"
        continue
    fi

    branch=$(git -C "$path" rev-parse --abbrev-ref HEAD 2>/dev/null || echo "?")
    last=$(git -C "$path" log -1 --oneline 2>/dev/null || echo "?")
    dirty=""
    if ! git -C "$path" diff --quiet 2>/dev/null || ! git -C "$path" diff --cached --quiet 2>/dev/null; then
        dirty=" [dirty]"
    fi

    printf '%-20s %-30s %-20s %s\n' "$name" "$label" "${branch}${dirty}" "$last"
done

printf '%0.s─' {1..90}; echo
