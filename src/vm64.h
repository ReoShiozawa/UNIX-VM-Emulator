#ifndef VM64_H
#define VM64_H

#include <stdint.h>
#include <stddef.h>
#include <sys/syscall.h>

/* Extended 64-bit VM with Linux compatibility */
#define VM64_RAM_SIZE (8 * 1024 * 1024)  /* 8 MB */
#define VM64_REG_COUNT 16                 /* RAX-R15 */

/* x86-64 Register indices */
typedef enum {
    RAX = 0, RCX = 1, RDX = 2, RBX = 3,
    RSP = 4, RBP = 5, RSI = 6, RDI = 7,
    R8 = 8, R9 = 9, R10 = 10, R11 = 11,
    R12 = 12, R13 = 13, R14 = 14, R15 = 15
} X86Reg;

/* Extended Opcode set */
typedef enum {
    /* Basic ops */
    X64_HALT = 0x00,
    X64_NOP = 0x01,
    
    /* Arithmetic */
    X64_MOVI = 0x10,
    X64_ADD = 0x11,
    X64_SUB = 0x12,
    X64_MUL = 0x13,
    X64_DIV = 0x14,
    X64_MOD = 0x15,
    
    /* Bitwise */
    X64_AND = 0x20,
    X64_OR = 0x21,
    X64_XOR = 0x22,
    X64_NOT = 0x23,
    X64_SHL = 0x24,
    X64_SHR = 0x25,
    
    /* Memory */
    X64_LOAD = 0x30,
    X64_STORE = 0x31,
    X64_MEMCPY = 0x32,
    
    /* I/O */
    X64_OUT = 0x40,
    X64_IN = 0x41,
    
    /* Control Flow */
    X64_JMP = 0x50,
    X64_JNZ = 0x51,
    X64_JZ = 0x52,
    X64_CALL = 0x53,
    X64_RET = 0x54,
    
    /* Linux syscall */
    X64_SYSCALL = 0x80,
    
    /* Stack */
    X64_PUSH = 0x90,
    X64_POP = 0x91,
} X64Opcode;

/* VM64 State */
typedef struct {
    uint8_t* ram;                          /* Dynamically allocated memory */
    uint64_t regs[VM64_REG_COUNT];        /* RAX-R15 */
    uint64_t rip;                          /* Instruction pointer */
    uint64_t rsp;                          /* Stack pointer */
    
    uint32_t eflags;                       /* EFLAGS register */
    
    int halted;                            /* Execution halted */
    uint64_t cycle_count;                  /* Total cycles executed */
    
    /* Statistics */
    uint64_t instruction_count;
    
    /* Debug */
    int debug_mode;
} VM64;

/* Function declarations */
VM64* vm64_create(void);
void vm64_destroy(VM64* vm);
void vm64_reset(VM64* vm);
int vm64_load_image(VM64* vm, const char* filename, uint64_t load_addr);
int vm64_load_kernel(VM64* vm, const char* filename);
void vm64_execute_one(VM64* vm);
void vm64_run(VM64* vm);
void vm64_dump_state(VM64* vm);
void vm64_set_debug(VM64* vm, int enable);

/* Linux syscall interface */
void vm64_syscall_handler(VM64* vm);

#endif /* VM64_H */
