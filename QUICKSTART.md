# UNIX VM Emulator Suite - Quick Start Guide

## 🚀 Start Here

```bash
cd /Users/kinoko/Documents/app/emulator
make                    # Build everything
./bin/launcher          # Run interactive launcher
```

## 📋 What You Get

| Component | Description | Command |
|-----------|-------------|---------|
| **Launcher** | Interactive menu for all tools | `./bin/launcher` |
| **8-bit VM** | Simple educational RISC emulator | `./bin/emulator` |
| **8-bit GUI** | Visual register display | `./bin/emulator-gui` |
| **x86-64 VM** | Advanced 64-bit emulator | `./bin/vm64` |
| **Image Gen** | Create binary images | `./bin/imggen` |
| **Ubuntu Download** | Ubuntu ISO downloader | Option 5 in launcher |
| **Ubuntu Launch** | Full Ubuntu desktop via QEMU | Option 6 in launcher |

## 🎯 Quick Examples

### 1. Run 8-bit Demo
```bash
./bin/launcher
→ Choose option 1
vm> help
vm> run
```

### 2. Run x86-64 VM
```bash
./bin/launcher
→ Choose option 3
x64> dump
x64> quit
```

### 3. Launch Ubuntu
```bash
./bin/launcher
→ Choose option 5 (Download Ubuntu ISO) - first time only
→ Choose option 6 (Launch Ubuntu)
# Ubuntu desktop opens!

# OR via direct commands:
./download-ubuntu.sh        # Download ISO (4GB, ~5 min)
./ubuntu-quick.sh           # Launch Ubuntu
```

## 📁 Project Structure

```
emulator/
├── bin/                         # Compiled executables
│   ├── launcher                 # Interactive menu
│   ├── emulator                 # 8-bit RISC VM
│   ├── emulator-gui             # 8-bit with GUI
│   └── vm64                     # x86-64 VM
├── src/                         # Source code
│   ├── launcher.c               # Menu system
│   ├── vm.h / vm.c              # 8-bit RISC
│   ├── vm64.h / vm64.c          # x86-64
│   ├── main.c / main64.c        # CLI interfaces
│   ├── gui.c                    # SDL2 graphics
│   └── imggen.c                 # Image generator
├── isos/                        # OS ISO storage
│   └── ubuntu-24.04.3-desktop-amd64.iso
├── qemu-images/                 # Virtual disks
│   └── ubuntu-disk.qcow2
├── Makefile                     # Build system
├── README.md                    # Full documentation
├── LAUNCHER_GUIDE.md            # Detailed guide
├── run-ubuntu.sh                # QEMU launcher
└── download-ubuntu.sh           # ISO downloader
```

## 🔧 Build Commands

```bash
make                    # Default: launcher + cli + vm64
make launcher           # Just the menu
make cli                # 8-bit emulator only
make vm64               # x86-64 emulator only
make gui                # With SDL2 graphics (requires: brew install sdl2)
make imggen             # Image generator tool
make clean              # Remove binaries
make help               # Show all options
```

## 💾 Installation Notes

**macOS (recommended):**
```bash
brew install gcc qemu sdl2
cd /Users/kinoko/Documents/app/emulator
make
./bin/launcher
```

**Linux (Debian/Ubuntu):**
```bash
sudo apt install build-essential qemu-system-x86 libsdl2-dev
cd /Users/kinoko/Documents/app/emulator
make
./bin/launcher
```

## 📚 Architecture Overview

### Tier 1: 8-bit RISC (Educational)
- Simple instruction interpreter
- 64 KB RAM, 8 registers, 28 opcodes
- Perfect for learning VM concepts

### Tier 2: x86-64 (Intermediate)
- More realistic CPU simulation
- 8 MB RAM, 16 registers, Linux syscalls
- Great for kernel development learning

### Tier 3: Full OS (Advanced)
- QEMU x86-64 emulator
- Complete Ubuntu desktop environment
- Production-ready emulation

## ❓ FAQ

**Q: How do I launch Ubuntu?**
A: Run `./download-ubuntu.sh` then `./bin/launcher` → choose option 5

**Q: Do I need SDL2?**
A: No - GUI is optional. CLI works without it.

**Q: Can this run real kernels?**
A: VM64 can load x86-64 ELF binaries with syscall support

**Q: How much disk space for Ubuntu?**
A: ~4 GB for ISO + 50 GB for virtual disk (created automatically)

## 🎓 Learning Resources

- **README.md** - Complete technical documentation
- **LAUNCHER_GUIDE.md** - Detailed usage guide
- **src/*.c** - Well-commented source code

## 🐛 Troubleshooting

**Launcher won't start:**
```bash
cd /Users/kinoko/Documents/app/emulator
make clean && make
./bin/launcher
```

**QEMU error:**
```bash
brew install qemu
```

**Ubuntu download failed:**
```bash
./download-ubuntu.sh  # Resume download automatically
```

## ✨ What's Implemented

✅ 8-bit RISC VM with 28 opcodes
✅ x86-64 VM with Linux syscalls
✅ Interactive debugging & stepping
✅ SDL2 GUI visualization
✅ Binary image generator
✅ QEMU integration
✅ Full Ubuntu support
✅ Pure C (no Python/dependencies)

---

**Ready?** `./bin/launcher` 🚀
