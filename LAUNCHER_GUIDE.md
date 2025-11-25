# Interactive Launcher Guide

## Quick Start

```bash
./bin/launcher
```

## Menu Overview

```
1) 8-bit RISC VM          - Simple 64KB educational emulator
2) 8-bit RISC VM + GUI    - Visual register/memory display (requires SDL2)
3) x86-64 VM with syscalls - Extended 8MB x86-64 emulator with Linux syscalls
4) Image Generator         - Generate binary images (hello, counter, fibonacci)
5) Ubuntu with QEMU        - Launch full Ubuntu ISO
0) Exit                    - Quit launcher
```

## Architecture

### Tier 1: 8-bit RISC VM (Educational)
- **CPU**: Simple 8×64-bit register architecture
- **Memory**: 64 KB RAM
- **Instruction Set**: 28 opcodes (arithmetic, bitwise, memory, I/O, control)
- **Use Case**: Learning VM fundamentals

### Tier 2: x86-64 VM (Intermediate)
- **CPU**: 16×64-bit registers (RAX-R15 compatible)
- **Memory**: 8 MB RAM (configurable)
- **Linux Syscalls**: write, read, open, close, exit, mmap, brk, lseek
- **Use Case**: Small kernel development, syscall learning

### Tier 3: QEMU Integration (Full OS)
- **Emulator**: QEMU x86-64 full system emulator
- **Support**: Complete Ubuntu/Linux desktop environments
- **Use Case**: Full operating system testing

## Example Workflows

### 1. Learn VM Architecture
```bash
./bin/launcher
→ Choose option 1 (8-bit RISC)
vm> help              # Show available commands
vm> dump              # View registers and memory
vm> step 5            # Execute 5 instructions
vm> run               # Execute until HALT
vm> debug on          # Enable debug tracing
```

### 2. Test Generated Image
```bash
./bin/launcher
→ Choose option 4 (Image Generator)
Enter: ./bin/imggen images/test.bin hello
→ Choose option 1 (8-bit RISC)
vm> load images/test.bin
vm> run
```

### 3. Run x86-64 Kernel
```bash
./bin/launcher
→ Choose option 3 (x86-64 VM)
x64> help
x64> load mykernel.bin
x64> run
```

### 4. Launch Ubuntu Desktop
```bash
./bin/launcher
→ Choose option 5 (Ubuntu with QEMU)
# Ensure ISO is available, QEMU will auto-download if needed
```

## Building Components

### Build Everything (Recommended)
```bash
make                  # Builds launcher, emulator, vm64
```

### Build Specific Components
```bash
make launcher         # Interactive menu only
make cli              # 8-bit VM only
make vm64             # x86-64 VM only
make gui              # GUI version (requires SDL2)
make imggen           # Image generator tool
```

### Install Dependencies

**macOS**:
```bash
brew install gcc qemu sdl2
```

**Linux (Debian/Ubuntu)**:
```bash
sudo apt install build-essential qemu-system-x86 libsdl2-dev
```

**Linux (Fedora)**:
```bash
sudo dnf install gcc SDL2-devel qemu-system-x86
```

## File Structure

```
emulator/
├── bin/
│   ├── launcher       ✓ Interactive menu
│   ├── emulator       ✓ 8-bit RISC CLI
│   ├── emulator-gui   ✓ 8-bit RISC with SDL2 GUI
│   ├── vm64           ✓ x86-64 emulator
│   └── imggen         ✓ Image generator
├── src/
│   ├── launcher.c     ← Interactive launcher menu
│   ├── vm.h/c         ← 8-bit RISC VM implementation
│   ├── vm64.h/c       ← x86-64 VM implementation
│   ├── main.c         ← 8-bit CLI interface
│   ├── main64.c       ← x86-64 CLI interface
│   ├── gui.c          ← SDL2 visualization
│   └── imggen.c       ← Binary image generator
├── images/            ← Generated sample binaries
├── Makefile           ← Build system
├── run-ubuntu.sh      ← QEMU launcher script
├── README.md          ← Full documentation
└── LAUNCHER_GUIDE.md  ← This file
```

## Advanced Usage

### Debug Mode
```bash
vm> debug on
vm> break 0x0100      # Set breakpoint
vm> run               # Run until breakpoint
vm> step 1            # Single step
vm> dump              # Inspect state
```

### Custom Images
```bash
# Create your own binary image
# Format: raw x86-64 machine code or RISC bytecode

./bin/vm64 mykernel.bin       # Run x86-64 binary
./bin/emulator myprogram.bin  # Run 8-bit RISC binary
```

### Network & GPU (via QEMU)
The `run-ubuntu.sh` script includes:
- **Network**: User-mode NAT with e1000 NIC
- **Audio**: SoundBlaster 16 audio device
- **Graphics**: GTK display (native QEMU GUI)

## Troubleshooting

**Launcher not found?**
```bash
cd /path/to/emulator
make clean && make launcher
```

**SDL2 errors?**
```bash
# Skip GUI, use CLI instead:
./bin/launcher
# Choose option 1 or 3
```

**QEMU not found?**
```bash
# Install QEMU:
brew install qemu            # macOS
sudo apt install qemu-system-x86  # Linux

# Then use Ubuntu option in launcher
```

**vm64 crashes?**
```bash
# Check if kernel binary is valid x86-64 ELF
# Reduce --memory if needed (default 8MB)
```

## Performance Notes

- **8-bit VM**: ~1-10 MIPS (simple interpreter)
- **x86-64 VM**: ~100-500 KIPS (larger instruction set)
- **QEMU**: Full native speed with KVM acceleration

## Educational Value

This launcher demonstrates:
1. **VM Architecture**: Multi-tier design (simple→advanced)
2. **Instruction Interpretation**: Direct bytecode execution
3. **System Integration**: Linux syscall emulation
4. **Full OS Support**: QEMU integration
5. **Interactive Debugging**: Step-by-step execution

Ideal for computer science students learning:
- Virtual machines
- CPU architecture
- Operating systems
- Systems programming
- Assembly language
