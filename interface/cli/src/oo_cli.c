#include "oo_proto.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#ifdef _WIN32
#include <direct.h>
FILE *popen(const char *command, const char *mode);
int pclose(FILE *stream);
#define OO_GETCWD _getcwd
#define OO_POPEN popen
#define OO_PCLOSE pclose
#define OO_PATH_SEP '\\'
#define OO_NULL_DEVICE "NUL"
#else
#include <unistd.h>
#define OO_GETCWD getcwd
#define OO_POPEN popen
#define OO_PCLOSE pclose
#define OO_PATH_SEP '/'
#define OO_NULL_DEVICE "/dev/null"
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

typedef struct {
    const char *name;
    const char *layer;
    const char *github;
    const char *label;
} OORepo;

static const OORepo OO_REPOS[] = {
    { "llm-baremetal", "1-cognitive", "Djiby-diop/llm-baremetal", "1 - Cognitive Core" },
    { "oo-host",       "2-kernel", "Djiby-diop/oo-host", "2 - Execution Kernel" },
    { "oo-sim",        "3-simulation", "Djiby-diop/oo-sim", "3 - Simulation" },
    { "oo-lab",        "4-research", "Djiby-diop/oo-lab", "4 - Research" },
    { "oo-dplus",      "5-evolution", "Djiby-diop/oo-dplus", "5 - Evolution" },
    { "oo-system",     "6+7-meta+iface", "Djiby-diop/oo-system", "6+7 - Meta + Interface" },
};

static void print_help(void);
static void cmd_status(int argc, char **argv);
static void cmd_think(int argc, char **argv);
static void cmd_journal(int argc, char **argv);
static void cmd_layers(void);
static int has_flag(int argc, char **argv, const char *short_flag, const char *long_flag);
static void join_args(int argc, char **argv, int start_index, char *buffer, size_t buffer_size);
static const char *oo_root(void);
static int path_exists(const char *path);
static int is_directory(const char *path);
static int path_join(const char *base, const char *part, char *buffer, size_t buffer_size);
static int path_join3(const char *part1, const char *part2, const char *part3, char *buffer, size_t buffer_size);
static int parent_path(char *path);
static void trim_trailing(char *value);
static int starts_with(const char *value, const char *prefix);
static int ends_with(const char *value, const char *suffix);
static int run_command_capture(const char *command, char *buffer, size_t buffer_size);
static void git_short_status(const char *path, char *buffer, size_t buffer_size);
static void git_last_commit(const char *path, char *buffer, size_t buffer_size);
static void print_divider(void);

int main(int argc, char **argv) {
    if (argc < 2) {
        print_help();
        return 0;
    }

    if (strcmp(argv[1], "status") == 0) {
        cmd_status(argc - 2, argv + 2);
        return 0;
    }
    if (strcmp(argv[1], "think") == 0) {
        cmd_think(argc - 2, argv + 2);
        return 0;
    }
    if (strcmp(argv[1], "journal") == 0) {
        cmd_journal(argc - 2, argv + 2);
        return 0;
    }
    if (strcmp(argv[1], "layers") == 0) {
        cmd_layers();
        return 0;
    }
    if (strcmp(argv[1], "help") == 0 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        print_help();
        return 0;
    }

    fprintf(stderr, "oo: unknown command '%s'. Run 'oo help'.\n", argv[1]);
    return 1;
}

static void cmd_status(int argc, char **argv) {
    size_t i;
    int verbose = has_flag(argc, argv, "-v", "--verbose");
    const char *root = oo_root();

    printf("OO Operating Organism - Status\n");
    print_divider();

    for (i = 0; i < sizeof(OO_REPOS) / sizeof(OO_REPOS[0]); ++i) {
        char repo_path[PATH_MAX];
        char status[256];

        if (!path_join(root, OO_REPOS[i].name, repo_path, sizeof(repo_path)) || !path_exists(repo_path)) {
            snprintf(status, sizeof(status), "NOT FOUND");
        } else {
            git_short_status(repo_path, status, sizeof(status));
        }

        printf("  %-30s  %s\n", OO_REPOS[i].label, status);
    }

    print_divider();

    if (verbose) {
        puts("\nGit log (last commit per repo):");
        for (i = 0; i < sizeof(OO_REPOS) / sizeof(OO_REPOS[0]); ++i) {
            char repo_path[PATH_MAX];
            char summary[512];

            if (path_join(root, OO_REPOS[i].name, repo_path, sizeof(repo_path)) && path_exists(repo_path)) {
                git_last_commit(repo_path, summary, sizeof(summary));
                printf("  %-20s  %s\n", OO_REPOS[i].name, summary);
            }
        }
    }
}

static void cmd_think(int argc, char **argv) {
    char text[1024];
    OOMessageHeader header;
    uint64_t timestamp_ns;

    if (argc <= 0) {
        fprintf(stderr, "Usage: oo think <text>\n");
        fprintf(stderr, "  Send a thought to the Cognitive Core via OO Message Bus.\n");
        exit(1);
    }

    join_args(argc, argv, 0, text, sizeof(text));
    timestamp_ns = (uint64_t)time(NULL) * 1000000000ULL;
    oo_msg_init(&header, OO_LAYER_INTERFACE, OO_LAYER_COGNITIVE, OO_EVENT_QUERY,
        0ULL, timestamp_ns, (uint32_t)strlen(text));

    puts("[OO] THINK -> Cognitive Core");
    printf("  Input: %s\n", text);
    printf("  Bus: %s -> %s (%s)\n",
        oo_layer_name((OOLayer)header.from),
        oo_layer_name((OOLayer)header.to),
        oo_event_name((OOEvent)header.kind));
    printf("  Payload bytes: %u\n", (unsigned int)header.payload_len);
    puts("  Status: bare-metal bridge not yet active (Phase 2).");
    puts("  Hint: boot llm-baremetal in QEMU and use /ssm_infer or the REPL.");
}

static void cmd_journal(int argc, char **argv) {
    char journal_dir[PATH_MAX];
    int cat = has_flag(argc, argv, NULL, "--cat");
    DIR *dir;
    struct dirent *entry;
    int found = 0;

    if (!path_join3("llm-baremetal", "build", "qemu-fs", journal_dir, sizeof(journal_dir))) {
        fprintf(stderr, "[journal] Unable to resolve journal directory.\n");
        exit(1);
    }

    dir = opendir(journal_dir);
    if (dir == NULL) {
        puts("[journal] No journal files found. Boot llm-baremetal to generate them.");
        puts("  Hint: cd llm-baremetal && make && ./scripts/run-qemu.sh");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        char file_path[PATH_MAX];
        FILE *file;
        char line[512];
        int printed_content = 0;

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        if (!starts_with(entry->d_name, "oo-journal") && !ends_with(entry->d_name, ".journal")) {
            continue;
        }
        if (!path_join(journal_dir, entry->d_name, file_path, sizeof(file_path))) {
            continue;
        }

        printf("[journal] %s\n", file_path);
        found = 1;

        if (!cat) {
            continue;
        }

        file = fopen(file_path, "r");
        if (file == NULL) {
            fprintf(stderr, "  unable to open %s: %s\n", file_path, strerror(errno));
            continue;
        }

        while (fgets(line, sizeof(line), file) != NULL) {
            fputs(line, stdout);
            printed_content = 1;
        }
        if (printed_content) {
            putchar('\n');
        }
        fclose(file);
    }

    closedir(dir);

    if (!found) {
        puts("[journal] No journal files found. Boot llm-baremetal to generate them.");
        puts("  Hint: cd llm-baremetal && make && ./scripts/run-qemu.sh");
    }
}

static void cmd_layers(void) {
    puts("OO Architecture - 7 Layers");
    print_divider();
    puts("  1. Cognitive Core      llm-baremetal       UEFI kernel + LLM + Mamba SSM");
    puts("  2. Execution Kernel    oo-host             Agent runtime + scheduling (Rust)");
    puts("  3. Simulation          oo-sim              Simulated worlds + behavior training (C)");
    puts("  4. Research            oo-lab              Prototypes + experiments (C)");
    puts("  5. Evolution           oo-dplus            D+ policy engine + mutation");
    puts("  6. Meta                oo-system/meta      Self-modification layer (C target)");
    puts("  7. Interface           oo-system/interface CLI + API + hardware bridge (C)");
    print_divider();
    puts("Detailed spec: oo-system/ARCHITECTURE.md");
}

static void print_help(void) {
    puts("oo - Operating Organism CLI v0.1");
    puts("");
    puts("USAGE:");
    puts("  oo <command> [args]");
    puts("");
    puts("COMMANDS:");
    puts("  status [-v]        Show status of all OO repos");
    puts("  think <text>       Send a thought to the Cognitive Core");
    puts("  journal [--cat]    Show OO journal files");
    puts("  layers             List all 7 OO architecture layers");
    puts("  help               Show this help");
    puts("");
    puts("ENVIRONMENT:");
    puts("  OO_ROOT            Path to OO workspace (default: auto-detected)");
    puts("");
    puts("BUILD:");
    puts("  make               Build shared/oo-proto and the oo CLI");
    puts("");
    puts("EXAMPLES:");
    puts("  oo status -v");
    puts("  oo think \"what is consciousness?\"");
    puts("  oo layers");
}

static int has_flag(int argc, char **argv, const char *short_flag, const char *long_flag) {
    int i;

    for (i = 0; i < argc; ++i) {
        if (short_flag != NULL && strcmp(argv[i], short_flag) == 0) {
            return 1;
        }
        if (long_flag != NULL && strcmp(argv[i], long_flag) == 0) {
            return 1;
        }
    }
    return 0;
}

static void join_args(int argc, char **argv, int start_index, char *buffer, size_t buffer_size) {
    int i;
    size_t used = 0;

    if (buffer_size == 0) {
        return;
    }

    buffer[0] = '\0';
    for (i = start_index; i < argc; ++i) {
        size_t part_len = strlen(argv[i]);
        if (used + part_len + 2 >= buffer_size) {
            break;
        }
        if (used != 0) {
            buffer[used++] = ' ';
        }
        memcpy(buffer + used, argv[i], part_len);
        used += part_len;
        buffer[used] = '\0';
    }
}

static const char *oo_root(void) {
    static char root[PATH_MAX];
    char probe[PATH_MAX];
    const char *env_root = getenv("OO_ROOT");
    int depth;

    if (env_root != NULL && env_root[0] != '\0' && strlen(env_root) < sizeof(root)) {
        snprintf(root, sizeof(root), "%s", env_root);
        return root;
    }

    if (OO_GETCWD(root, sizeof(root)) == NULL) {
        snprintf(root, sizeof(root), ".");
        return root;
    }

    for (depth = 0; depth < 8; ++depth) {
        if (path_join(root, "oo-system", probe, sizeof(probe)) && is_directory(probe)) {
            return root;
        }
        if (path_join(root, "llm-baremetal", probe, sizeof(probe)) && is_directory(probe)) {
            return root;
        }
        if (!parent_path(root)) {
            break;
        }
    }

    if (OO_GETCWD(root, sizeof(root)) == NULL) {
        snprintf(root, sizeof(root), ".");
    }
    return root;
}

static int path_exists(const char *path) {
    struct stat st;
    return path != NULL && stat(path, &st) == 0;
}

static int is_directory(const char *path) {
    struct stat st;
    return path != NULL && stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int path_join(const char *base, const char *part, char *buffer, size_t buffer_size) {
    int written;
    size_t base_len;

    if (base == NULL || part == NULL || buffer == NULL || buffer_size == 0) {
        return 0;
    }

    base_len = strlen(base);
    if (base_len == 0) {
        written = snprintf(buffer, buffer_size, "%s", part);
        return written > 0 && (size_t)written < buffer_size;
    }
    if (base[base_len - 1] == '/' || base[base_len - 1] == '\\') {
        written = snprintf(buffer, buffer_size, "%s%s", base, part);
        return written > 0 && (size_t)written < buffer_size;
    }
    written = snprintf(buffer, buffer_size, "%s%c%s", base, OO_PATH_SEP, part);
    return written > 0 && (size_t)written < buffer_size;
}

static int path_join3(const char *part1, const char *part2, const char *part3, char *buffer, size_t buffer_size) {
    char temp[PATH_MAX];
    char temp2[PATH_MAX];

    if (!path_join(oo_root(), part1, temp, sizeof(temp))) {
        return 0;
    }
    if (!path_join(temp, part2, temp2, sizeof(temp2))) {
        return 0;
    }
    return path_join(temp2, part3, buffer, buffer_size);
}

static int parent_path(char *path) {
    char *slash1;
    char *slash2;
    char *parent;

    if (path == NULL || path[0] == '\0') {
        return 0;
    }

    trim_trailing(path);
    slash1 = strrchr(path, '/');
    slash2 = strrchr(path, '\\');
    parent = slash1;
    if (slash2 != NULL && (parent == NULL || slash2 > parent)) {
        parent = slash2;
    }
    if (parent == NULL) {
        return 0;
    }

    if (parent == path) {
        path[1] = '\0';
        return 1;
    }

    if (parent == path + 2 && path[1] == ':') {
        path[3] = '\0';
        return 1;
    }

    *parent = '\0';
    return 1;
}

static void trim_trailing(char *value) {
    size_t len;

    if (value == NULL) {
        return;
    }

    len = strlen(value);
    while (len > 0 && (value[len - 1] == '\n' || value[len - 1] == '\r' ||
            isspace((unsigned char)value[len - 1]) || value[len - 1] == '/' || value[len - 1] == '\\')) {
        value[len - 1] = '\0';
        --len;
    }
}

static int starts_with(const char *value, const char *prefix) {
    size_t prefix_len;
    if (value == NULL || prefix == NULL) {
        return 0;
    }
    prefix_len = strlen(prefix);
    return strncmp(value, prefix, prefix_len) == 0;
}

static int ends_with(const char *value, const char *suffix) {
    size_t value_len;
    size_t suffix_len;

    if (value == NULL || suffix == NULL) {
        return 0;
    }
    value_len = strlen(value);
    suffix_len = strlen(suffix);
    if (suffix_len > value_len) {
        return 0;
    }
    return strcmp(value + value_len - suffix_len, suffix) == 0;
}

static int run_command_capture(const char *command, char *buffer, size_t buffer_size) {
    FILE *pipe;
    size_t used = 0;

    if (buffer == NULL || buffer_size == 0) {
        return 0;
    }
    buffer[0] = '\0';

    pipe = OO_POPEN(command, "r");
    if (pipe == NULL) {
        return 0;
    }

    while (fgets(buffer + used, (int)(buffer_size - used), pipe) != NULL) {
        used = strlen(buffer);
        if (used + 1 >= buffer_size) {
            break;
        }
    }

    OO_PCLOSE(pipe);
    trim_trailing(buffer);
    return buffer[0] != '\0';
}

static void git_short_status(const char *path, char *buffer, size_t buffer_size) {
    char command[PATH_MAX + 128];
    char branch[128];
    char porcelain[512];

    if (buffer == NULL || buffer_size == 0) {
        return;
    }

    snprintf(command, sizeof(command), "git -C \"%s\" rev-parse --abbrev-ref HEAD 2>%s", path, OO_NULL_DEVICE);
    if (!run_command_capture(command, branch, sizeof(branch))) {
        snprintf(buffer, buffer_size, "not a git repo");
        return;
    }

    snprintf(command, sizeof(command), "git -C \"%s\" status --porcelain 2>%s", path, OO_NULL_DEVICE);
    if (run_command_capture(command, porcelain, sizeof(porcelain)) && porcelain[0] != '\0') {
        snprintf(buffer, buffer_size, "branch=%s [dirty]", branch);
    } else {
        snprintf(buffer, buffer_size, "branch=%s [clean]", branch);
    }
}

static void git_last_commit(const char *path, char *buffer, size_t buffer_size) {
    char command[PATH_MAX + 128];

    if (buffer == NULL || buffer_size == 0) {
        return;
    }

    snprintf(command, sizeof(command), "git -C \"%s\" log -1 --oneline 2>%s", path, OO_NULL_DEVICE);
    if (!run_command_capture(command, buffer, buffer_size)) {
        snprintf(buffer, buffer_size, "(no commits)");
    }
}

static void print_divider(void) {
    puts("------------------------------------------------------------");
}