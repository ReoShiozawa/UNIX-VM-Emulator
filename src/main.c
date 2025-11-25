#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* CLI command structure */
typedef struct {
    const char* cmd;
    const char* help;
    void (*handler)(VM*, const char*);
} Command;

/* Forward declarations */
void cmd_help(VM* vm, const char* args);
void cmd_run(VM* vm, const char* args);
void cmd_step(VM* vm, const char* args);
void cmd_dump(VM* vm, const char* args);
void cmd_load(VM* vm, const char* args);
void cmd_reset(VM* vm, const char* args);
void cmd_debug(VM* vm, const char* args);
void cmd_break(VM* vm, const char* args);
void cmd_cont(VM* vm, const char* args);

/* Command list */
const Command commands[] = {
    {"help", "Show this help message", cmd_help},
    {"run", "Run the VM until HALT", cmd_run},
    {"step", "Execute one instruction", cmd_step},
    {"dump", "Dump VM state (registers, memory)", cmd_dump},
    {"load", "Load an image file: load <filename>", cmd_load},
    {"reset", "Reset VM to initial state", cmd_reset},
    {"debug", "Toggle debug mode: debug [on|off]", cmd_debug},
    {"break", "Add breakpoint: break <addr>", cmd_break},
    {"cont", "Continue from breakpoint", cmd_cont},
    {"quit", "Exit the emulator", NULL},
    {NULL, NULL, NULL}
};

void cmd_help(VM* vm, const char* args) {
    (void)vm;
    (void)args;
    printf("\n=== UNIX VM Emulator - CLI Help ===\n\n");
    for (int i = 0; commands[i].cmd; i++) {
        printf("  %-10s - %s\n", commands[i].cmd, commands[i].help);
    }
    printf("\n");
}

void cmd_run(VM* vm, const char* args) {
    (void)args;
    if (!vm) return;
    printf("Running VM...\n");
    vm_run(vm);
    printf("\nVM halted. Total cycles: %llu\n", vm->cycle_count);
}

void cmd_step(VM* vm, const char* args) {
    int count = 1;
    if (args) sscanf(args, "%d", &count);
    
    for (int i = 0; i < count && !vm->halted; i++) {
        vm_execute_one(vm);
    }
    printf("Executed %d instruction(s). PC: 0x%04X\n", count, vm->pc);
}

void cmd_dump(VM* vm, const char* args) {
    (void)args;
    vm_dump_state(vm);
}

void cmd_load(VM* vm, const char* args) {
    if (!args || strlen(args) == 0) {
        printf("Usage: load <filename>\n");
        return;
    }
    
    vm_reset(vm);
    if (vm_load_image(vm, args) == 0) {
        printf("Loaded image: %s\n", args);
    } else {
        printf("Failed to load image: %s\n", args);
    }
}

void cmd_reset(VM* vm, const char* args) {
    (void)args;
    vm_reset(vm);
    printf("VM reset to initial state\n");
}

void cmd_debug(VM* vm, const char* args) {
    int enable = -1;
    if (args) {
        if (strncmp(args, "on", 2) == 0) enable = 1;
        else if (strncmp(args, "off", 3) == 0) enable = 0;
    }
    
    if (enable >= 0) {
        vm_set_debug_mode(vm, enable);
        printf("Debug mode: %s\n", enable ? "ON" : "OFF");
    } else {
        printf("Debug mode is %s. Usage: debug [on|off]\n",
               vm->debug_mode ? "ON" : "OFF");
    }
}

void cmd_break(VM* vm, const char* args) {
    if (!args) {
        printf("Usage: break <address>\n");
        return;
    }
    
    uint16_t addr = 0;
    sscanf(args, "%hx", &addr);
    vm_add_breakpoint(vm, addr);
    printf("Breakpoint added at 0x%04X\n", addr);
}

void cmd_cont(VM* vm, const char* args) {
    (void)args;
    vm_run(vm);
}

void interactive_shell(VM* vm) {
    char line[256];
    
    printf("\n=== UNIX VM Emulator - Interactive CLI ===\n");
    printf("Type 'help' for available commands\n\n");
    
    while (1) {
        printf("vm> ");
        if (fgets(line, sizeof(line), stdin) == NULL) break;
        
        /* Remove trailing newline */
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
        }
        
        /* Skip empty lines */
        if (strlen(line) == 0) continue;
        
        /* Parse command and arguments */
        char cmd[64] = {0};
        char args[192] = {0};
        sscanf(line, "%63s %191[^\n]", cmd, args);
        
        /* Execute command */
        int found = 0;
        for (int i = 0; commands[i].cmd; i++) {
            if (strcmp(commands[i].cmd, cmd) == 0) {
                found = 1;
                if (strcmp(cmd, "quit") == 0) {
                    printf("Exiting...\n");
                    return;
                }
                if (commands[i].handler) {
                    commands[i].handler(vm, args);
                }
                break;
            }
        }
        
        if (!found) {
            printf("Unknown command: %s (try 'help')\n", cmd);
        }
    }
}

int main(int argc, char* argv[]) {
    VM* vm = vm_create();
    if (!vm) {
        fprintf(stderr, "Failed to create VM\n");
        return EXIT_FAILURE;
    }
    
    /* Load initial image */
    if (argc > 1) {
        if (vm_load_image(vm, argv[1]) != 0) {
            vm_destroy(vm);
            return EXIT_FAILURE;
        }
    } else {
        /* Load built-in demo */
        vm_load_builtin_image(vm);
    }
    
    /* Check if stdin is a terminal */
    if (isatty(fileno(stdin))) {
        /* Interactive mode */
        interactive_shell(vm);
    } else {
        /* Batch mode - just run */
        vm_run(vm);
    }
    
    vm_destroy(vm);
    return EXIT_SUCCESS;
}
