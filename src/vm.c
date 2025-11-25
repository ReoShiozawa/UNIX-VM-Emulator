#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Create a new VM instance */
VM* vm_create(void) {
    VM* vm = (VM*)malloc(sizeof(VM));
    if (!vm) return NULL;
    
    memset(vm, 0, sizeof(VM));
    vm->sp = VM_RAM_SIZE - 1;  /* Stack grows downward */
    vm->debug_mode = 0;
    vm->breakpoint_count = 0;
    
    return vm;
}

/* Destroy a VM instance */
void vm_destroy(VM* vm) {
    if (vm) free(vm);
}

/* Reset VM to initial state */
void vm_reset(VM* vm) {
    if (!vm) return;
    
    memset(vm->ram, 0, VM_RAM_SIZE);
    memset(vm->regs, 0, sizeof(vm->regs));
    vm->pc = 0;
    vm->sp = VM_RAM_SIZE - 1;
    vm->halted = 0;
    vm->cycle_count = 0;
}

/* Load a binary image from file */
int vm_load_image(VM* vm, const char* filename) {
    if (!vm || !filename) return -1;
    
    FILE* f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return -1;
    }
    
    size_t bytes_read = fread(vm->ram, 1, VM_RAM_SIZE, f);
    fclose(f);
    
    if (bytes_read == 0) {
        fprintf(stderr, "Error: File '%s' is empty\n", filename);
        return -1;
    }
    
    vm->pc = 0;
    return 0;
}

/* Load the built-in demo image */
int vm_load_builtin_image(VM* vm) {
    if (!vm) return -1;
    
    /* "HELLO FROM VM" demo - each character followed by OUT instruction */
    const uint8_t demo[] = {
        0x10, 0x00, 0x48, 0x00, 0x00, 0x00,  /* MOVI R0, 0x48 ('H') */
        0x40, 0x00,                            /* OUT R0 */
        0x10, 0x00, 0x45, 0x00, 0x00, 0x00,  /* MOVI R0, 0x45 ('E') */
        0x40, 0x00,                            /* OUT R0 */
        0x10, 0x00, 0x4C, 0x00, 0x00, 0x00,  /* MOVI R0, 0x4C ('L') */
        0x40, 0x00,                            /* OUT R0 */
        0x10, 0x00, 0x4C, 0x00, 0x00, 0x00,  /* MOVI R0, 0x4C ('L') */
        0x40, 0x00,                            /* OUT R0 */
        0x10, 0x00, 0x4F, 0x00, 0x00, 0x00,  /* MOVI R0, 0x4F ('O') */
        0x40, 0x00,                            /* OUT R0 */
        0x10, 0x00, 0x20, 0x00, 0x00, 0x00,  /* MOVI R0, 0x20 (' ') */
        0x40, 0x00,                            /* OUT R0 */
        0x10, 0x00, 0x46, 0x00, 0x00, 0x00,  /* MOVI R0, 0x46 ('F') */
        0x40, 0x00,                            /* OUT R0 */
        0x10, 0x00, 0x52, 0x00, 0x00, 0x00,  /* MOVI R0, 0x52 ('R') */
        0x40, 0x00,                            /* OUT R0 */
        0x10, 0x00, 0x4F, 0x00, 0x00, 0x00,  /* MOVI R0, 0x4F ('O') */
        0x40, 0x00,                            /* OUT R0 */
        0x10, 0x00, 0x4D, 0x00, 0x00, 0x00,  /* MOVI R0, 0x4D ('M') */
        0x40, 0x00,                            /* OUT R0 */
        0x10, 0x00, 0x20, 0x00, 0x00, 0x00,  /* MOVI R0, 0x20 (' ') */
        0x40, 0x00,                            /* OUT R0 */
        0x10, 0x00, 0x56, 0x00, 0x00, 0x00,  /* MOVI R0, 0x56 ('V') */
        0x40, 0x00,                            /* OUT R0 */
        0x10, 0x00, 0x4D, 0x00, 0x00, 0x00,  /* MOVI R0, 0x4D ('M') */
        0x40, 0x00,                            /* OUT R0 */
        0x10, 0x00, 0x0A, 0x00, 0x00, 0x00,  /* MOVI R0, 0x0A ('\n') */
        0x40, 0x00,                            /* OUT R0 */
        0x00                                   /* HALT */
    };
    
    size_t demo_size = sizeof(demo);
    if (demo_size > VM_RAM_SIZE) demo_size = VM_RAM_SIZE;
    
    memcpy(vm->ram, demo, demo_size);
    vm->pc = 0;
    
    return 0;
}

/* Execute a single instruction */
void vm_execute_one(VM* vm) {
    if (!vm || vm->halted || vm->pc >= VM_RAM_SIZE) {
        vm->halted = 1;
        return;
    }
    
    uint8_t opcode = vm->ram[vm->pc];
    
    if (vm->debug_mode) {
        printf("[PC: 0x%04X] Opcode: 0x%02X\n", vm->pc, opcode);
    }
    
    vm->pc++;
    vm->cycle_count++;
    
    switch (opcode) {
        case OP_HALT: {
            vm->halted = 1;
            break;
        }
        
        case OP_MOVI: {
            if (vm->pc + 4 >= VM_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t reg = vm->ram[vm->pc++];
            uint32_t imm = (vm->ram[vm->pc] << 24) |
                          (vm->ram[vm->pc+1] << 16) |
                          (vm->ram[vm->pc+2] << 8) |
                          vm->ram[vm->pc+3];
            vm->pc += 4;
            
            if (reg < VM_REG_COUNT) {
                vm->regs[reg] = imm;
            }
            break;
        }
        
        case OP_ADD: {
            if (vm->pc + 1 >= VM_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t dst = vm->ram[vm->pc++];
            uint8_t src = vm->ram[vm->pc++];
            
            if (dst < VM_REG_COUNT && src < VM_REG_COUNT) {
                vm->regs[dst] += vm->regs[src];
            }
            break;
        }
        
        case OP_SUB: {
            if (vm->pc + 1 >= VM_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t dst = vm->ram[vm->pc++];
            uint8_t src = vm->ram[vm->pc++];
            
            if (dst < VM_REG_COUNT && src < VM_REG_COUNT) {
                vm->regs[dst] -= vm->regs[src];
            }
            break;
        }
        
        case OP_MUL: {
            if (vm->pc + 1 >= VM_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t dst = vm->ram[vm->pc++];
            uint8_t src = vm->ram[vm->pc++];
            
            if (dst < VM_REG_COUNT && src < VM_REG_COUNT) {
                vm->regs[dst] *= vm->regs[src];
            }
            break;
        }
        
        case OP_DIV: {
            if (vm->pc + 1 >= VM_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t dst = vm->ram[vm->pc++];
            uint8_t src = vm->ram[vm->pc++];
            
            if (dst < VM_REG_COUNT && src < VM_REG_COUNT && vm->regs[src] != 0) {
                vm->regs[dst] /= vm->regs[src];
            }
            break;
        }
        
        case OP_MOD: {
            if (vm->pc + 1 >= VM_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t dst = vm->ram[vm->pc++];
            uint8_t src = vm->ram[vm->pc++];
            
            if (dst < VM_REG_COUNT && src < VM_REG_COUNT && vm->regs[src] != 0) {
                vm->regs[dst] %= vm->regs[src];
            }
            break;
        }
        
        case OP_AND: {
            if (vm->pc + 1 >= VM_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t dst = vm->ram[vm->pc++];
            uint8_t src = vm->ram[vm->pc++];
            
            if (dst < VM_REG_COUNT && src < VM_REG_COUNT) {
                vm->regs[dst] &= vm->regs[src];
            }
            break;
        }
        
        case OP_OR: {
            if (vm->pc + 1 >= VM_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t dst = vm->ram[vm->pc++];
            uint8_t src = vm->ram[vm->pc++];
            
            if (dst < VM_REG_COUNT && src < VM_REG_COUNT) {
                vm->regs[dst] |= vm->regs[src];
            }
            break;
        }
        
        case OP_XOR: {
            if (vm->pc + 1 >= VM_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t dst = vm->ram[vm->pc++];
            uint8_t src = vm->ram[vm->pc++];
            
            if (dst < VM_REG_COUNT && src < VM_REG_COUNT) {
                vm->regs[dst] ^= vm->regs[src];
            }
            break;
        }
        
        case OP_NOT: {
            if (vm->pc >= VM_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t dst = vm->ram[vm->pc++];
            
            if (dst < VM_REG_COUNT) {
                vm->regs[dst] = ~vm->regs[dst];
            }
            break;
        }
        
        case OP_SHL: {
            if (vm->pc + 1 >= VM_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t dst = vm->ram[vm->pc++];
            uint8_t imm = vm->ram[vm->pc++];
            
            if (dst < VM_REG_COUNT && imm < 64) {
                vm->regs[dst] <<= imm;
            }
            break;
        }
        
        case OP_SHR: {
            if (vm->pc + 1 >= VM_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t dst = vm->ram[vm->pc++];
            uint8_t imm = vm->ram[vm->pc++];
            
            if (dst < VM_REG_COUNT && imm < 64) {
                vm->regs[dst] >>= imm;
            }
            break;
        }
        
        case OP_LOAD: {
            if (vm->pc + 2 >= VM_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t dst = vm->ram[vm->pc++];
            uint16_t addr = (vm->ram[vm->pc] << 8) | vm->ram[vm->pc+1];
            vm->pc += 2;
            
            if (dst < VM_REG_COUNT && addr < VM_RAM_SIZE) {
                vm->regs[dst] = vm->ram[addr];
            }
            break;
        }
        
        case OP_STORE: {
            if (vm->pc + 2 >= VM_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t src = vm->ram[vm->pc++];
            uint16_t addr = (vm->ram[vm->pc] << 8) | vm->ram[vm->pc+1];
            vm->pc += 2;
            
            if (src < VM_REG_COUNT && addr < VM_RAM_SIZE) {
                vm->ram[addr] = (uint8_t)(vm->regs[src] & 0xFF);
            }
            break;
        }
        
        case OP_OUT: {
            if (vm->pc >= VM_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t reg = vm->ram[vm->pc++];
            
            if (reg < VM_REG_COUNT) {
                putchar((int)(vm->regs[reg] & 0xFF));
                fflush(stdout);
            }
            break;
        }
        
        case OP_IN: {
            if (vm->pc >= VM_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t reg = vm->ram[vm->pc++];
            
            if (reg < VM_REG_COUNT) {
                int ch = getchar();
                vm->regs[reg] = (ch != EOF) ? ch : 0;
            }
            break;
        }
        
        case OP_JMP: {
            if (vm->pc + 1 >= VM_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint16_t addr = (vm->ram[vm->pc] << 8) | vm->ram[vm->pc+1];
            vm->pc = addr;
            break;
        }
        
        case OP_JNZ: {
            if (vm->pc + 2 >= VM_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t reg = vm->ram[vm->pc++];
            uint16_t addr = (vm->ram[vm->pc] << 8) | vm->ram[vm->pc+1];
            vm->pc += 2;
            
            if (reg < VM_REG_COUNT && vm->regs[reg] != 0) {
                vm->pc = addr;
            }
            break;
        }
        
        case OP_JZ: {
            if (vm->pc + 2 >= VM_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t reg = vm->ram[vm->pc++];
            uint16_t addr = (vm->ram[vm->pc] << 8) | vm->ram[vm->pc+1];
            vm->pc += 2;
            
            if (reg < VM_REG_COUNT && vm->regs[reg] == 0) {
                vm->pc = addr;
            }
            break;
        }
        
        case OP_JLT: {
            if (vm->pc + 2 >= VM_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t reg = vm->ram[vm->pc++];
            uint16_t addr = (vm->ram[vm->pc] << 8) | vm->ram[vm->pc+1];
            vm->pc += 2;
            
            if (reg < VM_REG_COUNT && (int64_t)vm->regs[reg] < 0) {
                vm->pc = addr;
            }
            break;
        }
        
        case OP_JGT: {
            if (vm->pc + 2 >= VM_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t reg = vm->ram[vm->pc++];
            uint16_t addr = (vm->ram[vm->pc] << 8) | vm->ram[vm->pc+1];
            vm->pc += 2;
            
            if (reg < VM_REG_COUNT && (int64_t)vm->regs[reg] > 0) {
                vm->pc = addr;
            }
            break;
        }
        
        case OP_CMP: {
            if (vm->pc + 1 >= VM_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t dst = vm->ram[vm->pc++];
            uint8_t src = vm->ram[vm->pc++];
            
            if (dst < VM_REG_COUNT && src < VM_REG_COUNT) {
                /* Store comparison result in dst (0 if equal, non-zero if different) */
                vm->regs[dst] = (vm->regs[dst] != vm->regs[src]) ? 1 : 0;
            }
            break;
        }
        
        case OP_CALL: {
            if (vm->pc + 1 >= VM_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint16_t addr = (vm->ram[vm->pc] << 8) | vm->ram[vm->pc+1];
            vm->pc += 2;
            
            /* Push return address to stack */
            if (vm->sp > 1) {
                vm->sp -= 2;
                vm->ram[vm->sp] = (vm->pc >> 8) & 0xFF;
                vm->ram[vm->sp+1] = vm->pc & 0xFF;
                vm->pc = addr;
            }
            break;
        }
        
        case OP_RET: {
            /* Pop return address from stack */
            if (vm->sp + 1 < VM_RAM_SIZE) {
                vm->pc = (vm->ram[vm->sp] << 8) | vm->ram[vm->sp+1];
                vm->sp += 2;
            }
            break;
        }
        
        case OP_PUSH: {
            if (vm->pc >= VM_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t reg = vm->ram[vm->pc++];
            
            if (reg < VM_REG_COUNT && vm->sp > 7) {
                vm->sp -= 8;
                uint64_t val = vm->regs[reg];
                for (int i = 0; i < 8; i++) {
                    vm->ram[vm->sp + i] = (val >> (56 - i*8)) & 0xFF;
                }
            }
            break;
        }
        
        case OP_POP: {
            if (vm->pc >= VM_RAM_SIZE) {
                vm->halted = 1;
                break;
            }
            uint8_t reg = vm->ram[vm->pc++];
            
            if (reg < VM_REG_COUNT && vm->sp + 8 <= VM_RAM_SIZE) {
                uint64_t val = 0;
                for (int i = 0; i < 8; i++) {
                    val = (val << 8) | vm->ram[vm->sp + i];
                }
                vm->regs[reg] = val;
                vm->sp += 8;
            }
            break;
        }
        
        default:
            fprintf(stderr, "Unknown opcode: 0x%02X at PC 0x%04X\n", opcode, vm->pc - 1);
            vm->halted = 1;
    }
}

/* Run the VM until HALT */
void vm_run(VM* vm) {
    if (!vm) return;
    
    while (!vm->halted && vm->pc < VM_RAM_SIZE) {
        if (vm_at_breakpoint(vm)) {
            printf("\nBreakpoint hit at PC: 0x%04X\n", vm->pc);
            vm_dump_state(vm);
            break;
        }
        vm_execute_one(vm);
    }
}

/* Dump VM state for debugging */
void vm_dump_state(VM* vm) {
    if (!vm) return;
    
    printf("\n=== VM State ===\n");
    printf("PC: 0x%04X  SP: 0x%04X\n", vm->pc, vm->sp);
    printf("Cycles: %llu  Halted: %d\n", vm->cycle_count, vm->halted);
    printf("\nRegisters:\n");
    for (int i = 0; i < VM_REG_COUNT; i++) {
        printf("  R%d: 0x%016llX (%lld)\n", i, vm->regs[i], (long long)vm->regs[i]);
    }
    printf("\nStack (first 32 bytes):\n");
    for (int i = 0; i < 32 && vm->sp + i < VM_RAM_SIZE; i++) {
        printf("  [0x%04X]: 0x%02X\n", vm->sp + i, vm->ram[vm->sp + i]);
    }
}

/* Set debug mode */
void vm_set_debug_mode(VM* vm, int enable) {
    if (vm) vm->debug_mode = enable;
}

/* Add a breakpoint */
void vm_add_breakpoint(VM* vm, uint16_t addr) {
    if (!vm || vm->breakpoint_count >= VM_MAX_BREAKPOINTS) return;
    
    vm->breakpoints[vm->breakpoint_count++] = addr;
}

/* Remove a breakpoint */
void vm_remove_breakpoint(VM* vm, uint16_t addr) {
    if (!vm) return;
    
    for (int i = 0; i < vm->breakpoint_count; i++) {
        if (vm->breakpoints[i] == addr) {
            for (int j = i; j < vm->breakpoint_count - 1; j++) {
                vm->breakpoints[j] = vm->breakpoints[j+1];
            }
            vm->breakpoint_count--;
            break;
        }
    }
}

/* Check if at breakpoint */
int vm_at_breakpoint(VM* vm) {
    if (!vm) return 0;
    
    for (int i = 0; i < vm->breakpoint_count; i++) {
        if (vm->breakpoints[i] == vm->pc) {
            return 1;
        }
    }
    return 0;
}
