#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void print_menu(void) {
    printf("\n");
    printf("╔══════════════════════════════════════════╗\n");
    printf("║   UNIX VM Emulator Suite Launcher      ║\n");
    printf("╚══════════════════════════════════════════╝\n");
    printf("\n");
    printf("Emulators:\n");
    printf("  1) 8-bit RISC VM          (./bin/emulator)\n");
    printf("  2) 8-bit RISC VM + GUI    (./bin/emulator-gui)\n");
    printf("  3) x86-64 VM with syscalls (./bin/vm64)\n");
    printf("  4) Image Generator         (./bin/imggen)\n");
    printf("\n");
    printf("Ubuntu QEMU:\n");
    printf("  5) Download Ubuntu ISO    (./download-ubuntu.sh)\n");
    printf("  6) Launch Ubuntu          (./ubuntu-quick.sh)\n");
    printf("\n");
    printf("  0) Exit\n");
    printf("\n");
    printf("Enter choice [0-6]: ");
}

int main(int argc, char* argv[]) {
    char choice[10];
    const char* cwd = getcwd(NULL, 0);
    
    if (argc > 1) {
        /* Direct mode - run specified emulator */
        if (strcmp(argv[1], "vm64") == 0) {
            execvp("./bin/vm64", &argv[1]);
        } else if (strcmp(argv[1], "emulator") == 0) {
            execvp("./bin/emulator", &argv[1]);
        } else if (strcmp(argv[1], "gui") == 0) {
            execvp("./bin/emulator-gui", &argv[1]);
        } else if (strcmp(argv[1], "imggen") == 0) {
            execvp("./bin/imggen", &argv[1]);
        } else if (strcmp(argv[1], "download-ubuntu") == 0) {
            execvp("bash", (char* const[]){ "bash", "./download-ubuntu.sh", NULL });
        } else if (strcmp(argv[1], "ubuntu") == 0) {
            execvp("bash", (char* const[]){ "bash", "./ubuntu-quick.sh", NULL });
        }
        return 1;
    }
    
    /* Interactive menu mode */
    printf("\n╔════════════════════════════════════════════════════╗\n");
    printf("║  UNIX VM Emulator Suite - Interactive Launcher   ║\n");
    printf("║  Location: %s  ║\n", cwd);
    printf("╚════════════════════════════════════════════════════╝\n");
    
    while (1) {
        print_menu();
        
        if (fgets(choice, sizeof(choice), stdin) == NULL) {
            break;
        }
        
        switch (choice[0]) {
            case '1': {
                printf("\n=== 8-bit RISC VM ===\n");
                printf("Usage: emulator [image.bin]\n\n");
                execvp("./bin/emulator", (char* const[]){ "./bin/emulator", NULL });
                break;
            }
            
            case '2': {
                printf("\n=== 8-bit RISC VM with GUI ===\n");
                printf("Usage: emulator-gui [image.bin]\n\n");
                execvp("./bin/emulator-gui", (char* const[]){ "./bin/emulator-gui", NULL });
                break;
            }
            
            case '3': {
                printf("\n=== x86-64 VM with Linux Syscalls ===\n");
                printf("Usage: vm64 [kernel.bin]\n\n");
                execvp("./bin/vm64", (char* const[]){ "./bin/vm64", NULL });
                break;
            }
            
            case '4': {
                printf("\n=== Image Generator ===\n");
                printf("Usage: imggen output.bin [hello|counter|fibonacci]\n");
                printf("Example: ./bin/imggen images/hello.bin hello\n\n");
                
                char cmd[256];
                printf("Enter command: ");
                if (fgets(cmd, sizeof(cmd), stdin) != NULL) {
                    size_t len = strlen(cmd);
                    if (len > 0 && cmd[len-1] == '\n') {
                        cmd[len-1] = '\0';
                    }
                    system(cmd);
                }
                break;
            }
            
            case '5': {
                printf("\n=== Download Ubuntu ISO ===\n");
                printf("This will download Ubuntu 24.04.3 desktop edition (~4GB)\n");
                printf("Location: ./isos/ubuntu-24.04.3-desktop-amd64.iso\n\n");
                printf("Ensure you have curl or wget installed\n");
                printf("Download may take 10-30 minutes depending on your connection\n\n");
                
                printf("Starting download...\n");
                execvp("bash", (char* const[]){ "bash", "./download-ubuntu.sh", NULL });
                break;
            }
            
            case '6': {
                printf("\n=== Launch Ubuntu with QEMU ===\n");
                printf("This will launch Ubuntu using QEMU emulator.\n");
                printf("Ensure you have QEMU installed:\n");
                printf("  macOS: brew install qemu\n");
                printf("  Linux: sudo apt install qemu-system-x86\n\n");
                printf("Ensure Ubuntu ISO is downloaded:\n");
                printf("  Run option 5 first if you haven't downloaded the ISO\n\n");
                
                printf("Launching QEMU...\n");
                execvp("bash", (char* const[]){ "bash", "./ubuntu-quick.sh", NULL });
                break;
            }
            
            case '0':
                printf("Goodbye!\n");
                return 0;
            
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
    
    return 0;
}
