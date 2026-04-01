//! oo — Operating Organism CLI
//! Interact with the entire OO ecosystem from a single command.

use std::process::Command;
use std::path::{Path, PathBuf};

// ============================================================
// OO Repos registry (used by future `oo publish` and `oo sync` commands)
// ============================================================
#[allow(dead_code)]
struct OORepo {
    name:   &'static str,
    layer:  &'static str,
    github: &'static str,
}

#[allow(dead_code)]
const OO_REPOS: &[OORepo] = &[
    OORepo { name: "llm-baremetal", layer: "1-cognitive",  github: "Djiby-diop/llm-baremetal" },
    OORepo { name: "oo-host",       layer: "2-kernel",     github: "Djiby-diop/oo-host" },
    OORepo { name: "oo-sim",        layer: "3-simulation", github: "Djiby-diop/oo-sim" },
    OORepo { name: "oo-lab",        layer: "4-research",   github: "Djiby-diop/oo-lab" },
    OORepo { name: "oo-dplus",      layer: "5-evolution",  github: "Djiby-diop/oo-dplus" },
    OORepo { name: "oo-system",     layer: "6+7-meta+iface", github: "Djiby-diop/oo-system" },
];

// ============================================================
// Main
// ============================================================
fn main() {
    let args: Vec<String> = std::env::args().collect();
    if args.len() < 2 {
        print_help();
        return;
    }

    match args[1].as_str() {
        "status"  => cmd_status(&args[2..]),
        "think"   => cmd_think(&args[2..]),
        "journal" => cmd_journal(&args[2..]),
        "layers"  => cmd_layers(),
        "help" | "--help" | "-h" => print_help(),
        other => {
            eprintln!("oo: unknown command '{}'. Run 'oo help'.", other);
            std::process::exit(1);
        }
    }
}

// ============================================================
// Commands
// ============================================================

fn cmd_status(args: &[String]) {
    // Find OO root: walk up from cwd or use OO_ROOT env
    let root = oo_root();
    println!("OO Operating Organism — Status");
    println!("{}", "─".repeat(60));

    let repos = [
        ("llm-baremetal", "1 · Cognitive Core"),
        ("oo-host",       "2 · Execution Kernel"),
        ("oo-sim",        "3 · Simulation"),
        ("oo-lab",        "4 · Research"),
        ("oo-dplus",      "5 · Evolution"),
        ("oo-system",     "6+7 · Meta + Interface"),
    ];

    for (repo, label) in &repos {
        let path = root.join(repo);
        let exists = path.exists();
        let git_status = if exists {
            git_short_status(&path)
        } else {
            "NOT FOUND".to_string()
        };
        println!("  {:30}  {}", label, git_status);
    }

    println!("{}", "─".repeat(60));

    if args.contains(&"--verbose".to_string()) || args.contains(&"-v".to_string()) {
        println!("\nGit log (last commit per repo):");
        for (repo, _) in &repos {
            let path = root.join(repo);
            if path.exists() {
                let log = git_last_commit(&path);
                println!("  {:20}  {}", repo, log);
            }
        }
    }
}

fn cmd_think(args: &[String]) {
    if args.is_empty() {
        eprintln!("Usage: oo think <text>");
        eprintln!("  Send a thought to the Cognitive Core via OO Message Bus.");
        std::process::exit(1);
    }
    let text = args.join(" ");
    println!("[OO] THINK → Cognitive Core");
    println!("  Input: {}", text);
    println!("  Status: bare-metal bridge not yet active (Phase 2).");
    println!("  Hint: boot llm-baremetal in QEMU and use /ssm_infer or the REPL.");
}

fn cmd_journal(args: &[String]) {
    let root = oo_root();
    let journal_dirs = [
        root.join("llm-baremetal").join("build").join("qemu-fs"),
    ];
    let mut found = false;
    for dir in &journal_dirs {
        if dir.exists() {
            if let Ok(entries) = std::fs::read_dir(dir) {
                for entry in entries.flatten() {
                    let name = entry.file_name();
                    let name = name.to_string_lossy();
                    if name.starts_with("oo-journal") || name.ends_with(".journal") {
                        println!("[journal] {}", entry.path().display());
                        found = true;
                        if args.contains(&"--cat".to_string()) {
                            if let Ok(content) = std::fs::read_to_string(entry.path()) {
                                println!("{}", content);
                            }
                        }
                    }
                }
            }
        }
    }
    if !found {
        println!("[journal] No journal files found. Boot llm-baremetal to generate them.");
        println!("  Hint: cd llm-baremetal && make && ./scripts/run-qemu.sh");
    }
}

fn cmd_layers() {
    println!("OO Architecture — 7 Layers");
    println!("{}", "─".repeat(60));
    let layers = [
        ("1", "Cognitive Core",  "llm-baremetal", "UEFI kernel + LLM + Mamba SSM"),
        ("2", "Execution Kernel","oo-host",        "Agent runtime + scheduling (Rust)"),
        ("3", "Simulation",      "oo-sim",         "Simulated worlds + behavior training"),
        ("4", "Research",        "oo-lab",         "Prototypes + experiments"),
        ("5", "Evolution",       "oo-dplus",       "D+ policy engine + mutation"),
        ("6", "Meta",            "oo-system/meta", "OO modifies OO"),
        ("7", "Interface",       "oo-system/iface","CLI + API + hardware bridge"),
    ];
    for (n, name, repo, desc) in &layers {
        println!("  {}. {:20} {:25} {}", n, name, repo, desc);
    }
    println!("{}", "─".repeat(60));
    println!("Detailed spec: oo-system/ARCHITECTURE.md");
}

fn print_help() {
    println!("oo — Operating Organism CLI v0.1");
    println!();
    println!("USAGE:");
    println!("  oo <command> [args]");
    println!();
    println!("COMMANDS:");
    println!("  status [-v]        Show status of all OO repos");
    println!("  think <text>       Send a thought to the Cognitive Core");
    println!("  journal [--cat]    Show OO journal files");
    println!("  layers             List all 7 OO architecture layers");
    println!("  help               Show this help");
    println!();
    println!("ENVIRONMENT:");
    println!("  OO_ROOT            Path to OO workspace (default: auto-detected)");
    println!();
    println!("EXAMPLES:");
    println!("  oo status -v");
    println!("  oo think \"what is consciousness?\"");
    println!("  oo layers");
}

// ============================================================
// Helpers
// ============================================================

fn oo_root() -> PathBuf {
    // 1. OO_ROOT env var
    if let Ok(root) = std::env::var("OO_ROOT") {
        return PathBuf::from(root);
    }
    // 2. Walk up from cwd looking for oo-system or llm-baremetal
    let mut dir = std::env::current_dir().unwrap_or_else(|_| PathBuf::from("."));
    for _ in 0..8 {
        if dir.join("oo-system").exists() || dir.join("llm-baremetal").exists() {
            return dir;
        }
        if let Some(parent) = dir.parent() {
            dir = parent.to_path_buf();
        } else {
            break;
        }
    }
    // 3. Fallback: current dir
    std::env::current_dir().unwrap_or_else(|_| PathBuf::from("."))
}

fn git_short_status(path: &Path) -> String {
    let branch = Command::new("git")
        .args(["-C", path.to_str().unwrap_or("."), "rev-parse", "--abbrev-ref", "HEAD"])
        .output()
        .ok()
        .and_then(|o| String::from_utf8(o.stdout).ok())
        .unwrap_or_default()
        .trim()
        .to_string();

    let dirty = Command::new("git")
        .args(["-C", path.to_str().unwrap_or("."), "status", "--porcelain"])
        .output()
        .ok()
        .map(|o| !o.stdout.is_empty())
        .unwrap_or(false);

    if branch.is_empty() {
        return "not a git repo".to_string();
    }
    if dirty {
        format!("branch={} [dirty]", branch)
    } else {
        format!("branch={} [clean]", branch)
    }
}

fn git_last_commit(path: &Path) -> String {
    Command::new("git")
        .args(["-C", path.to_str().unwrap_or("."), "log", "-1", "--oneline"])
        .output()
        .ok()
        .and_then(|o| String::from_utf8(o.stdout).ok())
        .unwrap_or_default()
        .trim()
        .to_string()
}
