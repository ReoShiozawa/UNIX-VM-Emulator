#include "vm64.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

/* Linux syscall numbers (x86-64) - macOS compatibility */
#ifdef __APPLE__
#define SYS_write 4
#define SYS_read 3
#define SYS_open 2
#define SYS_close 6
#define SYS_lseek 19
#define SYS_exit 1
#define SYS_exit_group 231
#define SYS_mmap 9
#define SYS_brk 17
#endif

/* Create VM64 instance */
VM64* vm64_create(void) {
    VM64* vm = (VM64*)malloc(sizeof(VM64));
    if (!vm) return NULL;
    
    /* Initialize struct first */
    memset(vm, 0, sizeof(VM64));
    
    /* Allocate RAM */
    vm->ram = (uint8_t*)calloc(VM64_RAM_SIZE, 1);
    if (!vm->ram) {
        fprintf(stderr, "Error: Failed to allocate %llu bytes\n", 
                (unsigned long long)VM64_RAM_SIZE);
        free(vm);
        return NULL;
    }
    
    /* Initialize stack at top of memory */
    vm->rsp = VM64_RAM_SIZE - 8;  /* Align to 8 bytes */
    vm->eflags = 0x202;           /* IF | ZF */
    
    return vm;
}

/* Destroy VM64 */
void vm64_destroy(VM64* vm) {
    if (vm) {
        if (vm->ram) free(vm->ram);
        free(vm);
    }
}

/* Reset VM64 */
void vm64_reset(VM64* vm) {
    if (!vm) return;
    
    memset(vm->ram, 0, VM64_RAM_SIZE);
    memset(vm->regs, 0, sizeof(vm->regs));
    vm->rip = 0;
    vm->rsp = VM64_RAM_SIZE - 1;
    vm->eflags = 0x202;  /* IF | ZF */
    vm->halted = 0;
    vm->cycle_count = 0;
    vm->instruction_count = 0;
}

/* Load binary image at specified address */
int vm64_load_image(VM64* vm, const char* filename, uint64_t load_addr) {
    if (!vm || !filename || load_addr >= VM64_RAM_SIZE) return -1;
    
    FILE* f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return -1;
    }
    
    /* Get file size */
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    if (size <= 0 || load_addr + size > VM64_RAM_SIZE) {
        fprintf(stderr, "Error: File too large or invalid address\n");
        fclose(f);
        return -1;
    }
    
    size_t bytes_read = fread(&vm->ram[load_addr], 1, (size_t)size, f);
    fclose(f);
    
    if ((long)bytes_read != size) {
        fprintf(stderr, "Error: Failed to read entire file\n");
        return -1;
    }
    
    vm->rip = load_addr;
    printf("Loaded %zu bytes at 0x%llX\n", bytes_read, (unsigned long long)load_addr);
    
    return 0;
}

/* Load Linux kernel (currently placeholder) */
int vm64_load_kernel(VM64* vm, const char* filename) {
    /* For now, just load at standard kernel address */
    return vm64_load_image(vm, filename, 0x400000);
}

/* Linux syscall handler */
void vm64_syscall_handler(VM64* vm) {
    if (!vm) return;
    
    uint64_t syscall_id = vm->regs[RAX];
    
    /* Emulate common Linux x86-64 syscalls */
    switch (syscall_id) {
        case SYS_write: {
            /* write(fd, buf, count) */
            int fd = vm->regs[RDI];
            uint64_t buf_addr = vm->regs[RSI];
            uint64_t count = vm->regs[RDX];
            
            if (buf_addr + count > VM64_RAM_SIZE) {
                vm->regs[RAX] = -1;
                break;
            }
            
            ssize_t written = write(fd, &vm->ram[buf_addr], count);
            vm->regs[RAX] = written;
            break;
        }
        
        case SYS_read: {
            /* read(fd, buf, count) */
            int fd = vm->regs[RDI];
            uint64_t buf_addr = vm->regs[RSI];
            uint64_t count = vm->regs[RDX];
            
            if (buf_addr + count > VM64_RAM_SIZE) {
                vm->regs[RAX] = -1;
                break;
            }
            
            ssize_t n = read(fd, &vm->ram[buf_addr], count);
            vm->regs[RAX] = n;
            break;
        }
        
        case SYS_exit:
            vm->halted = 1;
            break;
        
        case SYS_exit_group:
            vm->halted = 1;
            break;
        
        case SYS_open: {
            /* open(filename, flags) - simplified */
            uint64_t filename_addr = vm->regs[RDI];
            int flags = vm->regs[RSI];
            
            if (filename_addr >= VM64_RAM_SIZE) {
                vm->regs[RAX] = -1;
                break;
            }
            
            int fd = open((char*)&vm->ram[filename_addr], flags, 0644);
            vm->regs[RAX] = fd;
            break;
        }
        
        case SYS_close: {
            /* close(fd) */
            int fd = vm->regs[RDI];
            int ret = close(fd);
            vm->regs[RAX] = ret;
            break;
        }
        
        case SYS_lseek: {
            /* lseek(fd, offset, whence) */
            int fd = vm->regs[RDI];
            off_t offset = vm->regs[RSI];
            int whence = vm->regs[RDX];
            
            off_t ret = lseek(fd, offset, whence);
            vm->regs[RAX] = ret;
            break;
        }
        
        case SYS_mmap: {
            /* mmap stub - just return the requested address */
            vm->regs[RAX] = vm->regs[RDI];
            break;
        }
        
        case SYS_brk: {
            /* brk stub */
            vm->regs[RAX] = vm->regs[RDI];
            break;
        }
        
        default:
            fprintf(stderr, "Unknown syscall: %llu\n", (unsigned long long)syscall_id);
            vm->regs[RAX] = -1;
    }
}

/* Execute one instruction */
void vm64_execute_one(VM64* vm) {
    if (!vm || vm->halted || vm->rip >= VM64_RAM_SIZE) {
        vm->halted = 1;
        return;
    }
    
    uint8_t opcode = vm->ram[vm->rip];
    vm->rip++;
    vm->cycle_count++;
    vm->instruction_count++;
    
    if (vm->debug_mode) {
        printf("[RIP: 0x%016llX] Opcode: 0x%02X\n",
               (unsigned long long)vm->rip - 1, opcode);
    }
    
    switch (opcode) {
        case X64_HALT:
            vm->halted = 1;
            break;
        
        case X64_NOP:
            break;
        
        case X64_MOVI: {
            if (vm->rip + 9 >= VM64_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            
            uint8_t reg = vm->ram[vm->rip++];
            uint64_t imm = 0;
            for (int i = 0; i < 8; i++) {
                imm = (imm << 8) | vm->ram[vm->rip++];
            }
            
            if (reg < VM64_REG_COUNT) {
                vm->regs[reg] = imm;
            }
            break;
        }
        
        case X64_ADD: {
            if (vm->rip + 1 >= VM64_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t dst = vm->ram[vm->rip++];
            uint8_t src = vm->ram[vm->rip++];
            
            if (dst < VM64_REG_COUNT && src < VM64_REG_COUNT) {
                vm->regs[dst] += vm->regs[src];
            }
            break;
        }
        
        case X64_SUB: {
            if (vm->rip + 1 >= VM64_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t dst = vm->ram[vm->rip++];
            uint8_t src = vm->ram[vm->rip++];
            
            if (dst < VM64_REG_COUNT && src < VM64_REG_COUNT) {
                vm->regs[dst] -= vm->regs[src];
            }
            break;
        }
        
        case X64_LOAD: {
            if (vm->rip + 2 >= VM64_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t dst = vm->ram[vm->rip++];
            uint64_t addr = 0;
            for (int i = 0; i < 8; i++) {
                addr = (addr << 8) | vm->ram[vm->rip++];
            }
            
            if (dst < VM64_REG_COUNT && addr < VM64_RAM_SIZE) {
                vm->regs[dst] = vm->ram[addr];
            }
            break;
        }
        
        case X64_STORE: {
            if (vm->rip + 2 >= VM64_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t src = vm->ram[vm->rip++];
            uint64_t addr = 0;
            for (int i = 0; i < 8; i++) {
                addr = (addr << 8) | vm->ram[vm->rip++];
            }
            
            if (src < VM64_REG_COUNT && addr < VM64_RAM_SIZE) {
                vm->ram[addr] = vm->regs[src] & 0xFF;
            }
            break;
        }
        
        case X64_OUT: {
            if (vm->rip >= VM64_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t reg = vm->ram[vm->rip++];
            
            if (reg < VM64_REG_COUNT) {
                putchar(vm->regs[reg] & 0xFF);
                fflush(stdout);
            }
            break;
        }
        
        case X64_SYSCALL:
            vm64_syscall_handler(vm);
            break;
        
        case X64_JMP: {
            if (vm->rip + 7 >= VM64_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint64_t addr = 0;
            for (int i = 0; i < 8; i++) {
                addr = (addr << 8) | vm->ram[vm->rip++];
            }
            vm->rip = addr;
            break;
        }
        
        case X64_PUSH: {
            if (vm->rip >= VM64_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t reg = vm->ram[vm->rip++];
            
            if (reg < VM64_REG_COUNT && vm->rsp > 7) {
                vm->rsp -= 8;
                uint64_t val = vm->regs[reg];
                for (int i = 0; i < 8; i++) {
                    vm->ram[vm->rsp + i] = (val >> (56 - i*8)) & 0xFF;
                }
            }
            break;
        }
        
        case X64_POP: {
            if (vm->rip >= VM64_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t reg = vm->ram[vm->rip++];
            
            if (reg < VM64_REG_COUNT && vm->rsp + 8 <= VM64_RAM_SIZE) {
                uint64_t val = 0;
                for (int i = 0; i < 8; i++) {
                    val = (val << 8) | vm->ram[vm->rsp + i];
                }
                vm->regs[reg] = val;
                vm->rsp += 8;
            }
            break;
        }
        
        default:
            fprintf(stderr, "Unknown opcode: 0x%02X at RIP 0x%llX\n",
                    opcode, (unsigned long long)vm->rip - 1);
            vm->halted = 1;
    }
}

/* Run VM64 */
void vm64_run(VM64* vm) {
    if (!vm) return;
    
    printf("Starting VM64 execution from RIP: 0x%llX\n",
           (unsigned long long)vm->rip);
    
    while (!vm->halted && vm->rip < VM64_RAM_SIZE) {
        vm64_execute_one(vm);
    }
    
    printf("\nVM64 halted\n");
    printf("Total instructions: %llu\n", (unsigned long long)vm->instruction_count);
    printf("Total cycles: %llu\n", (unsigned long long)vm->cycle_count);
}

/* Dump VM64 state */
void vm64_dump_state(VM64* vm) {
    if (!vm) return;
    
    printf("\n=== VM64 State ===\n");
    printf("RIP: 0x%016llX  RSP: 0x%016llX\n",
           (unsigned long long)vm->rip, (unsigned long long)vm->rsp);
    printf("EFLAGS: 0x%08X\n", vm->eflags);
    printf("Instructions: %llu  Cycles: %llu\n",
           (unsigned long long)vm->instruction_count,
           (unsigned long long)vm->cycle_count);
    
    printf("\nRegisters:\n");
    const char* reg_names[] = {
        "RAX", "RCX", "RDX", "RBX", "RSP", "RBP", "RSI", "RDI",
        "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15"
    };
    
    for (int i = 0; i < VM64_REG_COUNT; i++) {
        printf("  %3s: 0x%016llX (%lld)\n",
               reg_names[i], (unsigned long long)vm->regs[i],
               (long long)vm->regs[i]);
    }
}

/* Set debug mode */
void vm64_set_debug(VM64* vm, int enable) {
    if (vm) vm->debug_mode = enable;
}
