# OO — Operating Organism

> **Ce n'est pas un OS. Ce n'est pas une IA. C'est une plateforme d'émergence d'intelligence.**

[![llm-baremetal CI](https://img.shields.io/github/actions/workflow/status/Djiby-diop/llm-baremetal/ci.yml?label=cognitive-core&style=flat-square)](https://github.com/Djiby-diop/llm-baremetal)
[![oo-host CI](https://img.shields.io/github/actions/workflow/status/Djiby-diop/oo-host/oo-host-ci.yml?label=exec-kernel&style=flat-square)](https://github.com/Djiby-diop/oo-host)
[![oo-dplus CI](https://img.shields.io/github/actions/workflow/status/Djiby-diop/oo-dplus/oo-dplus-ci.yml?label=evolution&style=flat-square)](https://github.com/Djiby-diop/oo-dplus)

---

## Architecture — 7 couches

```
OO (Operating Organism)

  1. Cognitive Core     llm-baremetal   UEFI bare-metal + LLM + Mamba SSM
  2. Execution Kernel   oo-host         Agent runtime + scheduling (Rust)
  3. Simulation Layer   oo-sim          Mondes simulés + test comportements
  4. Research Layer     oo-lab          Prototypes + incubateur d'idées
  5. Evolution Layer    oo-dplus        D+ policy engine + mutation système
  6. Meta Layer         oo-system/meta  OO se modifie lui-même
  7. Interface Layer    oo-system/iface CLI `oo` + API + bridge hardware (C)
```

| Couche | Repo | Langage | Status |
|--------|------|---------|--------|
| Cognitive Core | [llm-baremetal](https://github.com/Djiby-diop/llm-baremetal) | C + UEFI | ✅ v0.1 |
| Execution Kernel | [oo-host](https://github.com/Djiby-diop/oo-host) | Rust | ✅ Actif |
| Simulation | [oo-sim](https://github.com/Djiby-diop/oo-sim) | C | 🟡 Phase 2 |
| Research | [oo-lab](https://github.com/Djiby-diop/oo-lab) | Multi | 🟡 Phase 2 |
| Evolution | [oo-dplus](https://github.com/Djiby-diop/oo-dplus) | Rust (no_std) | ✅ Actif |
| Meta | oo-system/meta | C | 🔲 Phase 4 |
| Interface | oo-system/interface | C | ✅ CLI actif |

---

## Démarrage rapide

```bash
# Cloner tout l'écosystème OO
./scripts/bootstrap.sh

# Voir l'état de tous les repos
./scripts/status.sh

# Build oo-system en C
make

# Build tout
./scripts/build-all.sh
```

---

## Ce qui rend OO unique

| OS classique | OO |
|---|---|
| Exécute des processus | Maintient des objectifs |
| Passif (attend des commandes) | Actif (raisonne en continu) |
| Stateless entre reboots | Mémoire persistante (KV + journal) |
| Programmé une fois | Évolue (D+ policy + Meta layer) |
| Séparé de l'IA | L'IA EST le kernel |

---

## Roadmap

- **Phase 1** — Structure + connexions inter-repos ← *nous sommes ici*
- **Phase 2** — Interface + Simulation (agents, oo-sim)
- **Phase 3** — Intelligence distribuée (memory loop, oo-lab pipeline)
- **Phase 4** — Meta Layer (OO se code lui-même)
- **Phase 5** — Autonomie complète (sovereign boot, multi-instance)

Voir [ARCHITECTURE.md](ARCHITECTURE.md) pour la spec complète.

---

> *"Pas un logiciel. Un organisme."*
> — OO Constitution, Article 0
