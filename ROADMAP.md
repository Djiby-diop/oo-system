# OO Roadmap

> Ordre strict : on ne passe pas à la phase suivante sans avoir stabilisé la précédente.
> Principe : Immunité → Stabilité → Adaptation → Extension → Mutation

---

## Phase 1 — Structure + Connexions (en cours)

**Objectif :** Tous les repos existent, compilent, ont un CI, et peuvent se parler.

- [x] `llm-baremetal` : kernel UEFI + LLM + Mamba SSM engine complet
- [x] `oo-host` : runtime Rust + CI GitHub Actions
- [x] `oo-dplus` : D+ policy engine + CI GitHub Actions
- [x] `oo-sim` : repo initialisé sur GitHub
- [x] `oo-lab` : repo initialisé sur GitHub
- [x] `oo-system` : repo umbrella + ARCHITECTURE.md
- [ ] `shared/oo-proto` : types OOMessage (Rust crate + C header)
- [ ] `interface/cli` : commande `oo status`
- [ ] `scripts/bootstrap.sh` : clone tous les repos en une commande
- [ ] CI umbrella : vérifie que tous les repos compilent

---

## Phase 2 — Interface + Simulation

**Objectif :** OO peut être interrogé depuis l'extérieur. oo-sim tourne.

- [ ] `interface/cli` complet : `oo think`, `oo journal`, `oo status`
- [ ] Bridge série : oo-host lit le serial log de QEMU en temps réel
- [ ] `oo-sim` : premier monde simulé (agent simple + policy D+)
- [ ] `oo-lab` : premier prototype (micro-benchmark Mamba vs llama2)
- [ ] QEMU test automatisé avec modèle stories15M.q8_0.gguf (Option A)

---

## Phase 3 — Intelligence Distribuée

**Objectif :** OO se souvient, apprend, planifie.

- [ ] Memory system : KV persist cross-reboot (COLD zone → FAT32)
- [ ] Reasoning loop : `OBSERVE → THINK → ACT → JOURNAL` automatique
- [ ] oo-lab pipeline : prototype → oo-host en one-command
- [ ] oo-sim → oo-dplus feedback : résultats de sim nourrissent les policies
- [ ] oo-host agent scheduler v2 : priorités dynamiques

---

## Phase 4 — Meta Layer

**Objectif :** OO peut proposer des modifications sur lui-même.

- [ ] `meta/reflect` : lire sa propre structure (repos, dépendances, taille)
- [ ] `meta/patch` : générer des diffs contrôlés (D+ confidence ≥ 0.9)
- [ ] `meta/evolve` : boucle `simulate → learn → modify → redeploy`
- [ ] Journal des auto-modifications (immuable)
- [ ] Mode SAFE (propose seulement) vs MODE LIVE (applique)

---

## Phase 5 — Autonomie Complète

**Objectif :** OO démarre, raisonne et évolue sans intervention humaine.

- [ ] Sovereign boot : séquence autonome sans shell humain
- [ ] Multi-instance : plusieurs OO qui coopèrent via message bus
- [ ] Distributed D+ : consensus entre instances pour les décisions critiques
- [ ] Self-scheduling : OO planifie ses propres tâches de recherche
- [ ] Auto-publication : OO ouvre des issues/PRs sur ses propres repos

---

*"La roadmap est vivante. Elle évolue avec OO."*
