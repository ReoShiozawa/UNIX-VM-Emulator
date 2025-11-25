#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <SDL.h>

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 680
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 16
#define OUTPUT_ROWS 30
#define OUTPUT_COLS 120

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* font_texture;
    
    /* Output buffer */
    char output[OUTPUT_ROWS][OUTPUT_COLS];
    int output_line;
    int output_col;
    
    /* Execution state */
    int running;
    int paused;
    int step_mode;
    int speed;  /* Cycles per frame */
} GUI;

/* Simple font rendering (8x16 bitmap font) */
void render_char(GUI* gui, char c, int x, int y, SDL_Color color) {
    SDL_Rect dst = {x, y, CHAR_WIDTH, CHAR_HEIGHT};
    
    /* Create a simple rectangle for each character */
    if (c >= 32 && c < 127) {
        SDL_SetRenderDrawColor(gui->renderer, color.r, color.g, color.b, 255);
        SDL_RenderFillRect(gui->renderer, &dst);
        
        /* Draw character using simple lines (ASCII art style) */
        SDL_SetRenderDrawColor(gui->renderer, 
                              255 - color.r, 255 - color.g, 255 - color.b, 255);
        SDL_RenderDrawRect(gui->renderer, &dst);
    }
}

void gui_init(GUI* gui) {
    memset(gui, 0, sizeof(GUI));
    memset(gui->output, ' ', sizeof(gui->output));
    gui->running = 1;
    gui->speed = 100;  /* 100 cycles per frame */
}

int gui_create_window(GUI* gui) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return -1;
    }
    
    gui->window = SDL_CreateWindow(
        "UNIX VM Emulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    
    if (!gui->window) {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    
    gui->renderer = SDL_CreateRenderer(
        gui->window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    
    if (!gui->renderer) {
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(gui->window);
        SDL_Quit();
        return -1;
    }
    
    SDL_SetRenderDrawColor(gui->renderer, 0, 0, 0, 255);
    
    return 0;
}

void gui_close(GUI* gui) {
    if (gui->renderer) SDL_DestroyRenderer(gui->renderer);
    if (gui->window) SDL_DestroyWindow(gui->window);
    SDL_Quit();
}

void gui_output_char(GUI* gui, char c) {
    if (c == '\n') {
        gui->output_line++;
        gui->output_col = 0;
        if (gui->output_line >= OUTPUT_ROWS) {
            /* Scroll up */
            for (int i = 0; i < OUTPUT_ROWS - 1; i++) {
                memcpy(gui->output[i], gui->output[i+1], OUTPUT_COLS);
            }
            memset(gui->output[OUTPUT_ROWS-1], ' ', OUTPUT_COLS);
            gui->output_line = OUTPUT_ROWS - 1;
        }
    } else if (c >= 32 && c < 127) {
        if (gui->output_col < OUTPUT_COLS) {
            gui->output[gui->output_line][gui->output_col++] = c;
        }
    }
}

void gui_render(GUI* gui, VM* vm) {
    SDL_SetRenderDrawColor(gui->renderer, 0, 0, 0, 255);
    SDL_RenderClear(gui->renderer);
    
    SDL_Color text_color = {0, 255, 0};  /* Green on black */
    SDL_Color bg_color = {0, 0, 0};
    
    /* Render output */
    int y_offset = 10;
    for (int row = 0; row < OUTPUT_ROWS; row++) {
        for (int col = 0; col < OUTPUT_COLS && col < 100; col++) {
            char c = gui->output[row][col];
            if (c != ' ') {
                int x = 10 + col * CHAR_WIDTH;
                int y = y_offset + row * CHAR_HEIGHT;
                render_char(gui, c, x, y, text_color);
            }
        }
    }
    
    /* Render register state */
    int reg_y = y_offset + OUTPUT_ROWS * CHAR_HEIGHT + 20;
    SDL_Color reg_color = {200, 200, 200};
    
    /* Draw register info (text-based) */
    char reg_str[512];
    snprintf(reg_str, sizeof(reg_str),
            "PC: 0x%04X | SP: 0x%04X | Cycles: %llu | %s",
            vm->pc, vm->sp, vm->cycle_count,
            vm->halted ? "HALTED" : "RUNNING");
    
    /* Simple text rendering by displaying register values */
    SDL_Rect status_rect = {10, reg_y, WINDOW_WIDTH - 20, 30};
    SDL_SetRenderDrawColor(gui->renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(gui->renderer, &status_rect);
    
    /* Render registers */
    int reg_y_base = reg_y + 40;
    for (int i = 0; i < VM_REG_COUNT; i++) {
        snprintf(reg_str, sizeof(reg_str), "R%d: 0x%016llX", i, vm->regs[i]);
        /* Each register gets a small area */
        int reg_x = 10 + (i % 4) * (WINDOW_WIDTH / 4);
        int reg_y_val = reg_y_base + (i / 4) * 30;
        
        SDL_Rect reg_rect = {reg_x, reg_y_val, WINDOW_WIDTH/4 - 5, 25};
        SDL_SetRenderDrawColor(gui->renderer, 30, 30, 50, 255);
        SDL_RenderFillRect(gui->renderer, &reg_rect);
        SDL_SetRenderDrawColor(gui->renderer, 100, 150, 200, 255);
        SDL_RenderDrawRect(gui->renderer, &reg_rect);
    }
    
    SDL_RenderPresent(gui->renderer);
}

void gui_handle_events(GUI* gui, VM* vm) {
    SDL_Event event;
    
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                gui->running = 0;
                break;
            
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                        gui->paused = !gui->paused;
                        break;
                    case SDLK_s:
                        if (gui->paused) {
                            vm_execute_one(vm);
                        }
                        break;
                    case SDLK_r:
                        vm_reset(vm);
                        memset(gui->output, ' ', sizeof(gui->output));
                        gui->output_line = 0;
                        gui->output_col = 0;
                        gui->paused = 0;
                        break;
                    case SDLK_ESCAPE:
                        gui->running = 0;
                        break;
                    case SDLK_UP:
                        gui->speed = (gui->speed < 1000) ? gui->speed + 100 : 1000;
                        printf("Speed: %d cycles/frame\n", gui->speed);
                        break;
                    case SDLK_DOWN:
                        gui->speed = (gui->speed > 1) ? gui->speed - 100 : 1;
                        printf("Speed: %d cycles/frame\n", gui->speed);
                        break;
                    default:
                        break;
                }
                break;
        }
    }
}

int main(int argc, char* argv[]) {
    GUI gui;
    gui_init(&gui);
    
    VM* vm = vm_create();
    if (!vm) {
        fprintf(stderr, "Failed to create VM\n");
        return EXIT_FAILURE;
    }
    
    /* Load image */
    if (argc > 1) {
        if (vm_load_image(vm, argv[1]) != 0) {
            fprintf(stderr, "Failed to load image: %s\n", argv[1]);
            vm_destroy(vm);
            return EXIT_FAILURE;
        }
    } else {
        vm_load_builtin_image(vm);
    }
    
    /* Create GUI window */
    if (gui_create_window(&gui) != 0) {
        vm_destroy(vm);
        return EXIT_FAILURE;
    }
    
    /* Main loop */
    printf("=== UNIX VM Emulator GUI ===\n");
    printf("Controls:\n");
    printf("  SPACE - Pause/Resume\n");
    printf("  S     - Step (when paused)\n");
    printf("  R     - Reset\n");
    printf("  UP    - Increase speed\n");
    printf("  DOWN  - Decrease speed\n");
    printf("  ESC   - Quit\n\n");
    
    while (gui.running) {
        gui_handle_events(&gui, vm);
        
        /* Execute cycles */
        if (!gui.paused && !vm->halted) {
            for (int i = 0; i < gui.speed && !vm->halted; i++) {
                uint8_t opcode = vm->ram[vm->pc];
                
                /* Capture output in GUI */
                if (opcode == OP_OUT) {
                    vm->pc++;
                    uint8_t reg = vm->ram[vm->pc];
                    if (reg < VM_REG_COUNT) {
                        gui_output_char(&gui, (char)(vm->regs[reg] & 0xFF));
                    }
                    vm->pc++;
                    vm->cycle_count++;
                } else {
                    vm_execute_one(vm);
                }
            }
        }
        
        gui_render(&gui, vm);
    }
    
    gui_close(&gui);
    vm_destroy(vm);
    
    printf("\nVM Halted. Final cycles: %llu\n", vm->cycle_count);
    
    return EXIT_SUCCESS;
}
