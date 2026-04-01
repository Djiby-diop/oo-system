#!/usr/bin/env bash
# OO Build All — build toute la stack OO
# Usage: bash build-all.sh [workspace_dir]

set -e

WORKSPACE="${1:-$(dirname "$(dirname "$0")")/..}"
WORKSPACE="$(cd "$WORKSPACE" && pwd)"
echo "=== OO Build All ==="
echo "Workspace: $WORKSPACE"
FAILED=()

# ─── 1. llm-baremetal (C + UEFI) ───────────────────────────
echo ""
echo "[1/5] llm-baremetal (C kernel + SSM engine)"
if [ -d "$WORKSPACE/llm-baremetal" ]; then
    (cd "$WORKSPACE/llm-baremetal" && make 2>&1 | tail -5) && echo "  ✓ llm-baremetal OK" || { echo "  ✗ llm-baremetal FAILED"; FAILED+=("llm-baremetal"); }
else
    echo "  ✗ llm-baremetal not found (run bootstrap.sh first)"
    FAILED+=("llm-baremetal")
fi

# ─── 2. oo-host (Rust) ─────────────────────────────────────
echo ""
echo "[2/5] oo-host (Rust runtime)"
if [ -d "$WORKSPACE/oo-host" ]; then
    (cd "$WORKSPACE/oo-host" && cargo build 2>&1 | tail -3) && echo "  ✓ oo-host OK" || { echo "  ✗ oo-host FAILED"; FAILED+=("oo-host"); }
fi

# ─── 3. oo-dplus (Rust no_std) ─────────────────────────────
echo ""
echo "[3/5] oo-dplus (Rust policy engine)"
if [ -d "$WORKSPACE/oo-dplus" ]; then
    (cd "$WORKSPACE/oo-dplus" && cargo build --features std 2>&1 | tail -3) && echo "  ✓ oo-dplus OK" || { echo "  ✗ oo-dplus FAILED"; FAILED+=("oo-dplus"); }
fi

# ─── 4. oo-system/interface/cli ────────────────────────────
echo ""
echo "[4/5] oo CLI (interface/cli)"
CLI_DIR="$WORKSPACE/oo-system/interface/cli"
if [ -d "$CLI_DIR" ]; then
    (cd "$CLI_DIR" && cargo build 2>&1 | tail -3) && echo "  ✓ oo CLI OK" || { echo "  ✗ oo CLI FAILED"; FAILED+=("oo-cli"); }
fi

# ─── 5. oo-system/shared/oo-proto ──────────────────────────
echo ""
echo "[5/5] oo-proto (shared types)"
PROTO_DIR="$WORKSPACE/oo-system/shared/oo-proto"
if [ -d "$PROTO_DIR" ]; then
    (cd "$PROTO_DIR" && cargo check 2>&1 | tail -3) && echo "  ✓ oo-proto OK" || { echo "  ✗ oo-proto FAILED"; FAILED+=("oo-proto"); }
fi

# ─── Summary ───────────────────────────────────────────────
echo ""
echo "═══════════════════════════════════════"
if [ ${#FAILED[@]} -eq 0 ]; then
    echo "✓ All builds passed"
else
    echo "✗ Failed: ${FAILED[*]}"
    exit 1
fi
