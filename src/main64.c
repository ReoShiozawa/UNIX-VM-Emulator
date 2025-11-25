#include "vm64.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void print_help(void) {
    printf("\n=== VM64 x86-64 Linux Emulator ===\n\n");
    printf("Commands:\n");
    printf("  help           - Show this help\n");
    printf("  load <file>    - Load binary at default address (0x400000)\n");
    printf("  load <file> <addr> - Load binary at specified address\n");
    printf("  run            - Execute until halt\n");
    printf("  dump           - Show VM state\n");
    printf("  debug [on|off] - Toggle debug mode\n");
    printf("  reset          - Reset VM\n");
    printf("  quit           - Exit\n\n");
}

void interactive_loop(VM64* vm) {
    char line[512];
    
    printf("VM64> ");
    fflush(stdout);
    
    while (fgets(line, sizeof(line), stdin) != NULL) {
        /* Remove trailing newline */
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
            len--;
        }
        
        /* Skip empty lines */
        if (len == 0) {
            printf("VM64> ");
            fflush(stdout);
            continue;
        }
        
        /* Parse command and arguments */
        char cmd[64] = {0};
        char arg1[256] = {0};
        char arg2[64] = {0};
        
        int parsed = sscanf(line, "%63s %255s %63s", cmd, arg1, arg2);
        if (parsed < 1) {
            printf("VM64> ");
            fflush(stdout);
            continue;
        }
        
        /* Execute command */
        if (strcmp(cmd, "help") == 0) {
            print_help();
        } else if (strcmp(cmd, "load") == 0) {
            if (strlen(arg1) == 0) {
                printf("Usage: load <filename> [address]\n");
            } else {
                uint64_t addr = 0x400000;  /* Default */
                if (strlen(arg2) > 0) {
                    sscanf(arg2, "%llx", (unsigned long long*)&addr);
                }
                if (vm64_load_image(vm, arg1, addr) != 0) {
                    printf("Failed to load %s\n", arg1);
                }
            }
        } else if (strcmp(cmd, "run") == 0) {
            printf("Running...\n");
            vm64_run(vm);
        } else if (strcmp(cmd, "dump") == 0) {
            vm64_dump_state(vm);
        } else if (strcmp(cmd, "debug") == 0) {
            if (strlen(arg1) > 0) {
                if (strcmp(arg1, "on") == 0) {
                    vm64_set_debug(vm, 1);
                    printf("Debug ON\n");
                } else if (strcmp(arg1, "off") == 0) {
                    vm64_set_debug(vm, 0);
                    printf("Debug OFF\n");
                }
            } else {
                printf("Debug is %s\n", vm->debug_mode ? "ON" : "OFF");
            }
        } else if (strcmp(cmd, "reset") == 0) {
            vm64_reset(vm);
            printf("VM reset\n");
        } else if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "exit") == 0) {
            printf("Goodbye!\n");
            break;
        } else {
            printf("Unknown command: %s (try 'help')\n", cmd);
        }
        
        printf("VM64> ");
        fflush(stdout);
    }
}

int main(int argc, char* argv[]) {
    VM64* vm = vm64_create();
    if (!vm) {
        fprintf(stderr, "Failed to create VM64\n");
        return EXIT_FAILURE;
    }
    
    printf("=== VM64 x86-64 Linux Emulator ===\n");
    printf("Memory: %d MB\n", (int)(VM64_RAM_SIZE / (1024*1024)));
    printf("Registers: RAX-R15 (16 x 64-bit)\n");
    printf("Linux syscall support: write, read, open, close, exit, mmap, brk\n\n");
    
    /* Load image if provided */
    if (argc > 1) {
        uint64_t addr = 0x400000;
        if (argc > 2) {
            sscanf(argv[2], "%llx", (unsigned long long*)&addr);
        }
        
        if (vm64_load_image(vm, argv[1], addr) == 0) {
            vm64_run(vm);
        }
    } else {
        /* Interactive mode */
        print_help();
        interactive_loop(vm);
    }
    
    vm64_destroy(vm);
    return EXIT_SUCCESS;
}
