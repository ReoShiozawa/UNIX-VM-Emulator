CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99
LDFLAGS = -lm
SDL2_CFLAGS = $(shell sdl2-config --cflags 2>/dev/null)
SDL2_LDFLAGS = $(shell sdl2-config --libs 2>/dev/null)

# Directories
SRC_DIR = src
BIN_DIR = bin

# Source files
VM_SOURCES = $(SRC_DIR)/vm.c
CLI_SOURCES = $(VM_SOURCES) $(SRC_DIR)/main.c
GUI_SOURCES = $(VM_SOURCES) $(SRC_DIR)/gui.c
VM64_SOURCES = $(SRC_DIR)/vm64.c
CLI64_SOURCES = $(VM64_SOURCES) $(SRC_DIR)/main64.c

# Object files
VM_OBJS = $(VM_SOURCES:.c=.o)
CLI_OBJS = $(CLI_SOURCES:.c=.o)
GUI_OBJS = $(GUI_SOURCES:.c=.o)
VM64_OBJS = $(VM64_SOURCES:.c=.o)
CLI64_OBJS = $(CLI64_SOURCES:.c=.o)

# Targets
CLI_TARGET = $(BIN_DIR)/emulator
GUI_TARGET = $(BIN_DIR)/emulator-gui
IMGGEN_TARGET = $(BIN_DIR)/imggen
CLI64_TARGET = $(BIN_DIR)/vm64
LAUNCHER_TARGET = $(BIN_DIR)/launcher
LAUNCHER_SOURCES = $(SRC_DIR)/launcher.c
LAUNCHER_OBJS = $(LAUNCHER_SOURCES:.c=.o)

# Default target
all: launcher cli vm64

# CLI target
cli: $(CLI_TARGET)

$(CLI_TARGET): $(CLI_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Built: $@"

# Launcher target
launcher: $(LAUNCHER_TARGET)

$(LAUNCHER_TARGET): $(LAUNCHER_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Built: $@"

# GUI target (requires SDL2)
gui: $(GUI_TARGET)

$(GUI_TARGET): $(GUI_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(SDL2_LDFLAGS)
	@echo "Built: $@"

# Image generator target
imggen: $(IMGGEN_TARGET)

$(IMGGEN_TARGET): $(SRC_DIR)/imggen.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "Built: $@"

# VM64 target (x86-64 Linux emulator)
vm64: $(CLI64_TARGET)

$(CLI64_TARGET): $(CLI64_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Built: $@"

# Object files
%.o: %.c
	$(CC) $(CFLAGS) $(SDL2_CFLAGS) -c -o $@ $<

# Clean
clean:
	rm -f $(VM_OBJS) $(CLI_OBJS) $(GUI_OBJS) $(VM64_OBJS) $(CLI64_OBJS) $(LAUNCHER_OBJS)
	rm -f $(CLI_TARGET) $(GUI_TARGET) $(IMGGEN_TARGET) $(CLI64_TARGET) $(LAUNCHER_TARGET)
	@echo "Cleaned."

# Help
help:
	@echo "=== UNIX VM Emulator Build System ==="
	@echo ""
	@echo "Targets:"
	@echo "  all         - Build launcher + CLI + VM64 (default)"
	@echo "  launcher    - Build interactive launcher menu"
	@echo "  cli         - Build CLI emulator (64KB RAM, 8 registers)"
	@echo "  gui         - Build GUI emulator (requires SDL2)"
	@echo "  imggen      - Build image generator tool"
	@echo "  vm64        - Build VM64 (8MB RAM, x86-64, Linux syscalls)"
	@echo "  clean       - Remove built files"
	@echo "  help        - Show this help"
	@echo ""
	@echo "Usage:"
	@echo "  make launcher             # Build interactive launcher"
	@echo "  make cli                  # Build 8-bit emulator"
	@echo "  make vm64                 # Build x86-64 emulator"
	@echo "  make gui                  # Build GUI (requires: brew install sdl2)"
	@echo "  make imggen               # Build image generator"
	@echo ""
	@echo "Running:"
	@echo "  ./bin/launcher            # Interactive launcher menu"
	@echo "  ./bin/emulator            # 8-bit VM - run built-in demo"
	@echo "  ./bin/vm64 kernel.bin     # x86-64 VM - load and run kernel"
	@echo "  ./run-ubuntu.sh"
	@echo ""

.PHONY: all launcher cli gui imggen vm64 clean help
