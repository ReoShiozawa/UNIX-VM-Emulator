#ifndef VM_H
#define VM_H

#include <stdint.h>
#include <stddef.h>

/* VM Configuration */
#define VM_RAM_SIZE (64 * 1024)  /* 64 KiB */
#define VM_REG_COUNT 8
#define VM_MAX_BREAKPOINTS 16

/* Opcode definitions */
typedef enum {
    OP_HALT    = 0x00,  /* HALT */
    OP_MOVI    = 0x10,  /* MOVI reg, imm32 */
    OP_ADD     = 0x20,  /* ADD dst, src */
    OP_SUB     = 0x21,  /* SUB dst, src */
    OP_MUL     = 0x22,  /* MUL dst, src */
    OP_DIV     = 0x23,  /* DIV dst, src */
    OP_MOD     = 0x24,  /* MOD dst, src */
    OP_AND     = 0x25,  /* AND dst, src */
    OP_OR      = 0x26,  /* OR dst, src */
    OP_XOR     = 0x27,  /* XOR dst, src */
    OP_NOT     = 0x28,  /* NOT dst */
    OP_SHL     = 0x29,  /* SHL dst, imm8 */
    OP_SHR     = 0x2A,  /* SHR dst, imm8 */
    OP_LOAD    = 0x30,  /* LOAD dst, addr16 */
    OP_STORE   = 0x31,  /* STORE src, addr16 */
    OP_OUT     = 0x40,  /* OUT reg */
    OP_IN      = 0x41,  /* IN reg (input from stdin) */
    OP_JMP     = 0x50,  /* JMP addr16 */
    OP_JNZ     = 0x51,  /* JNZ reg, addr16 */
    OP_JZ      = 0x52,  /* JZ reg, addr16 */
    OP_JLT     = 0x53,  /* JLT reg, addr16 */
    OP_JGT     = 0x54,  /* JGT reg, addr16 */
    OP_CMP     = 0x60,  /* CMP dst, src */
    OP_CALL    = 0x70,  /* CALL addr16 */
    OP_RET     = 0x71,  /* RET */
    OP_PUSH    = 0x80,  /* PUSH reg */
    OP_POP     = 0x81,  /* POP reg */
} Opcode;

/* VM State */
typedef struct {
    uint8_t ram[VM_RAM_SIZE];      /* Memory */
    uint64_t regs[VM_REG_COUNT];   /* R0-R7 */
    uint16_t pc;                   /* Program counter */
    uint16_t sp;                   /* Stack pointer */
    int halted;                    /* Execution halted */
    uint64_t cycle_count;          /* Total cycles executed */
    
    /* Debug info */
    int debug_mode;
    uint16_t breakpoints[VM_MAX_BREAKPOINTS];
    int breakpoint_count;
} VM;

/* Function declarations */
VM* vm_create(void);
void vm_destroy(VM* vm);
void vm_reset(VM* vm);
int vm_load_image(VM* vm, const char* filename);
int vm_load_builtin_image(VM* vm);
void vm_execute_one(VM* vm);
void vm_run(VM* vm);
void vm_dump_state(VM* vm);
void vm_set_debug_mode(VM* vm, int enable);
void vm_add_breakpoint(VM* vm, uint16_t addr);
void vm_remove_breakpoint(VM* vm, uint16_t addr);
int vm_at_breakpoint(VM* vm);

#endif /* VM_H */
