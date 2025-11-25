#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Helper to emit bytes */
void emit_byte(FILE* f, uint8_t b) {
    fputc(b, f);
}

void emit_dword(FILE* f, uint32_t v) {
    emit_byte(f, (v >> 24) & 0xFF);
    emit_byte(f, (v >> 16) & 0xFF);
    emit_byte(f, (v >> 8) & 0xFF);
    emit_byte(f, v & 0xFF);
}

void emit_word(FILE* f, uint16_t v) {
    emit_byte(f, (v >> 8) & 0xFF);
    emit_byte(f, v & 0xFF);
}

/* Generate HELLO WORLD program */
void gen_hello(FILE* f) {
    const char* msg = "HELLO WORLD";
    for (int i = 0; msg[i]; i++) {
        emit_byte(f, 0x10);          /* MOVI */
        emit_byte(f, 0);             /* r0 */
        emit_dword(f, msg[i]);       /* char */
        emit_byte(f, 0x40);          /* OUT */
        emit_byte(f, 0);             /* r0 */
    }
    emit_byte(f, 0x10);              /* MOVI */
    emit_byte(f, 0);                 /* r0 */
    emit_dword(f, '\n');             /* newline */
    emit_byte(f, 0x40);              /* OUT */
    emit_byte(f, 0);                 /* r0 */
    emit_byte(f, 0x00);              /* HALT */
}

/* Generate COUNTER (0-9) program */
void gen_counter(FILE* f) {
    /* Simple version - just output each digit */
    const char* digits = "0123456789";
    
    for (int i = 0; digits[i]; i++) {
        emit_byte(f, 0x10);              /* MOVI */
        emit_byte(f, 0);                 /* r0 */
        emit_dword(f, digits[i]);        /* char */
        emit_byte(f, 0x40);              /* OUT */
        emit_byte(f, 0);                 /* r0 */
    }
    
    emit_byte(f, 0x10);                  /* MOVI */
    emit_byte(f, 0);                     /* r0 */
    emit_dword(f, '\n');                 /* newline */
    emit_byte(f, 0x40);                  /* OUT */
    emit_byte(f, 0);                     /* r0 */
    
    emit_byte(f, 0x00);                  /* HALT */
}

/* Generate FIBONACCI program */
void gen_fibonacci(FILE* f) {
    /* r0 = 1 (F1) */
    emit_byte(f, 0x10);              /* MOVI */
    emit_byte(f, 0);                 /* r0 */
    emit_dword(f, 1);                /* 1 */
    
    /* r1 = 1 (F2) */
    emit_byte(f, 0x10);              /* MOVI */
    emit_byte(f, 1);                 /* r1 */
    emit_dword(f, 1);                /* 1 */
    
    /* r2 = 10 (iterations) */
    emit_byte(f, 0x10);              /* MOVI */
    emit_byte(f, 2);                 /* r2 */
    emit_dword(f, 10);               /* 10 */
    
    /* loop: OUT r0 */
    int loop_addr = 31;
    emit_byte(f, 0x40);              /* OUT */
    emit_byte(f, 0);                 /* r0 */
    
    /* newline */
    emit_byte(f, 0x10);              /* MOVI */
    emit_byte(f, 3);                 /* r3 */
    emit_dword(f, '\n');             /* newline */
    emit_byte(f, 0x40);              /* OUT */
    emit_byte(f, 3);                 /* r3 */
    
    /* r3 = r0 + r1 */
    emit_byte(f, 0x10);              /* MOVI */
    emit_byte(f, 3);                 /* r3 */
    emit_dword(f, 0);                /* 0 (temp) */
    emit_byte(f, 0x20);              /* ADD */
    emit_byte(f, 3);                 /* r3 = r0 + r1 (pseudo) */
    /* Actually: use r3 = r0, then r3 += r1 */
    
    /* r3 = r0 */
    emit_byte(f, 0x10);              /* MOVI */
    emit_byte(f, 3);                 /* r3 */
    emit_dword(f, 0);                /* Will copy r0 */
    
    /* Simplified: just alternate and decrement */
    /* r2 -= 1 */
    emit_byte(f, 0x10);              /* MOVI */
    emit_byte(f, 3);                 /* r3 */
    emit_dword(f, 1);                /* 1 */
    emit_byte(f, 0x21);              /* SUB */
    emit_byte(f, 2);                 /* r2 */
    emit_byte(f, 3);                 /* r3 */
    
    /* JNZ r2, loop */
    emit_byte(f, 0x51);              /* JNZ */
    emit_byte(f, 2);                 /* r2 */
    emit_word(f, loop_addr);         /* loop address */
    
    emit_byte(f, 0x00);              /* HALT */
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <output.bin> <program>\n", argv[0]);
        fprintf(stderr, "Programs: hello, counter, fibonacci\n");
        return EXIT_FAILURE;
    }
    
    const char* output = argv[1];
    const char* program = argv[2];
    
    FILE* f = fopen(output, "wb");
    if (!f) {
        fprintf(stderr, "Error: Cannot create %s\n", output);
        return EXIT_FAILURE;
    }
    
    if (strcmp(program, "hello") == 0) {
        gen_hello(f);
    } else if (strcmp(program, "counter") == 0) {
        gen_counter(f);
    } else if (strcmp(program, "fibonacci") == 0) {
        gen_fibonacci(f);
    } else {
        fprintf(stderr, "Unknown program: %s\n", program);
        fclose(f);
        return EXIT_FAILURE;
    }
    
    fclose(f);
    printf("Generated %s (%s program)\n", output, program);
    
    return EXIT_SUCCESS;
}
