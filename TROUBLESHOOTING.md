# Troubleshooting Guide

## QEMU Issues

### "Failed to get write lock" Error
**Problem:** QEMU can't access the virtual disk because another instance is running.

**Solution:**
```bash
# Kill all QEMU processes
pkill -9 qemu-system-x86_64

# Delete the locked disk image (will be recreated)
rm -f qemu-images/ubuntu-disk.qcow2

# Try launching again
./ubuntu-quick.sh
```

### "QEMU is not installed"
**Problem:** QEMU binary not found.

**Solution:**
```bash
# macOS
brew install qemu

# Linux (Debian/Ubuntu)
sudo apt install qemu-system-x86

# Linux (Fedora)
sudo dnf install qemu-system-x86
```

### "ISO file not found"
**Problem:** Ubuntu ISO not in expected location.

**Solutions:**
```bash
# Download it
./download-ubuntu.sh

# Or point to custom location
./ubuntu-quick.sh /custom/path/to/ubuntu.iso
```

### QEMU window won't open / appears to hang
**Problem:** QEMU started but no display window appears.

**Solution:**
```bash
# This is normal - QEMU may take 30+ seconds to boot
# Wait at least 60 seconds before giving up
# If it still doesn't appear, check system resources:
top  # See if qemu-system-x86_64 is using CPU

# If hanging, kill and try with less memory:
# Edit ubuntu-quick.sh and change -m 8G to -m 4G
pkill -9 qemu-system-x86_64
```

### Ubuntu installer is very slow
**Problem:** Emulation is slow or disk I/O is bottlenecked.

**Solutions:**
1. **Allocate more RAM** - Edit ubuntu-quick.sh, increase `-m` value
2. **Use fewer cores** - Change `-smp cores=4` to `-smp cores=2`
3. **Check system** - Close other applications to free resources
4. **Try writeback cache** - Already enabled in latest version

### "Permission denied" when running scripts
**Problem:** Scripts aren't executable.

**Solution:**
```bash
chmod +x ubuntu-quick.sh run-ubuntu.sh download-ubuntu.sh
```

---

## 8-bit RISC VM Issues

### "Emulator not found"
**Problem:** Binary wasn't built.

**Solution:**
```bash
make clean
make cli
./bin/emulator
```

### "Image file not found"
**Problem:** Custom binary image can't be loaded.

**Solution:**
```bash
# Generate a sample image first
make imggen
./bin/imggen images/test.bin hello

# Then load it
./bin/emulator images/test.bin
```

---

## x86-64 VM Issues

### "vm64 not found"
**Problem:** x86-64 VM wasn't built.

**Solution:**
```bash
make vm64
./bin/vm64
```

### Kernel won't load
**Problem:** Binary kernel loading fails.

**Solution:**
```bash
# Ensure it's a valid x86-64 ELF binary
file your-kernel.bin

# Try with built-in demo first
./bin/vm64
x64> help
```

---

## GUI Issues

### "SDL2 not found" 
**Problem:** GUI emulator can't find SDL2 library.

**Solution:**
```bash
# Install SDL2
brew install sdl2        # macOS
sudo apt install libsdl2-dev  # Linux

# Rebuild GUI
make clean
make gui
./bin/emulator-gui
```

### GUI window doesn't render properly
**Problem:** Graphics display issues.

**Solution:**
```bash
# Use CLI mode instead (always works)
./bin/emulator images/hello.bin

# Or reduce display resolution in SDL settings
```

---

## Build Issues

### "gcc: command not found"
**Problem:** C compiler not installed.

**Solution:**
```bash
# macOS
brew install gcc

# Linux (Debian/Ubuntu)
sudo apt install build-essential

# Linux (Fedora)
sudo dnf install gcc
```

### Make error: "No rule to make target"
**Problem:** Makefile has issues.

**Solution:**
```bash
# Clean and rebuild completely
make clean
make all
```

### Compilation warnings
**Problem:** Non-fatal compiler warnings appear.

**Status:** Expected - these are benign (macro redefinition warnings)
**Action:** Ignore - all functionality works correctly

---

## Launcher Menu Issues

### Launcher crashes or exits immediately
**Problem:** Interactive menu fails.

**Solution:**
```bash
# Rebuild launcher
make clean
make launcher

# Try directly
./bin/launcher

# Or skip launcher and run emulator directly
./bin/emulator
```

### Menu options don't work
**Problem:** Selecting options does nothing.

**Solution:**
1. Ensure you're in the emulator directory
2. Make sure binaries exist: `ls bin/`
3. Make sure scripts are executable: `ls -la *.sh`
4. Try running components directly

---

## Performance Issues

### Ubuntu boots very slowly
**Problem:** QEMU emulation is slow.

**Expected:** First boot takes 30-60 seconds
**Normal:** Subsequent boots are faster (~10 seconds)

**Solutions if truly slow:**
```bash
# 1. Allocate more memory (edit ubuntu-quick.sh)
# Change: -m 8G to -m 16G (if you have it)

# 2. Use fewer cores
# Change: -smp cores=4 to -smp cores=2

# 3. Check system resources aren't exhausted
top
```

### VM64 runs very slowly
**Problem:** x86-64 interpreter isn't optimized.

**Status:** This is expected - it's a direct interpreter, not JIT
**Solutions:**
1. Use simpler test programs
2. Check for infinite loops in your code
3. Profile with `debug on` and `step` commands

---

## Disk Space Issues

### "No space left on device"
**Problem:** Virtual disk or system running out of space.

**Solution:**
```bash
# Check available space
df -h

# Delete old virtual disk to save space
rm qemu-images/ubuntu-disk.qcow2

# Reduce allocated disk size (edit ubuntu-quick.sh)
# Change: 50G to 30G
```

### ISO download incomplete
**Problem:** Download was interrupted.

**Solution:**
```bash
# Resume download automatically
./download-ubuntu.sh
# It will continue from where it left off
```

---

## Network Issues

### Can't access internet from Ubuntu
**Problem:** QEMU networking not working.

**Status:** User-mode NAT network should work automatically

**Solutions:**
```bash
# Inside Ubuntu, check network
ip addr

# If no ip, try dhclient
sudo dhclient

# Or configure in Settings → Network
```

### Can't SSH into Ubuntu
**Problem:** SSH port forwarding not working.

**Solution:**
```bash
# Ensure Ubuntu has openssh-server installed
# (It comes pre-installed in desktop edition)

# Try SSH from host
ssh -p 2222 localhost

# Default credentials
# User: ubuntu
# Password: (blank on first run, set during install)
```

---

## Data Loss / Persistence Issues

### Ubuntu changes don't persist
**Problem:** Changes lost after reboot.

**Solution:**
```bash
# This shouldn't happen - virtual disk should persist
# Check disk exists:
ls -lh qemu-images/ubuntu-disk.qcow2

# If missing, disk was deleted
# Recreate and reinstall:
./ubuntu-quick.sh
```

### Lost virtual disk accidentally
**Problem:** Deleted `qemu-images/ubuntu-disk.qcow2`

**Solution:**
```bash
# Recreate (will be empty):
./ubuntu-quick.sh

# You'll need to reinstall Ubuntu
```

---

## Reporting Issues

If you encounter problems not listed here:

1. **Check system resources:**
   ```bash
   sysctl -n hw.memsize  # RAM
   df -h                 # Disk space
   ```

2. **Enable verbose output:**
   ```bash
   ./ubuntu-quick.sh 2>&1 | tee debug.log
   ```

3. **Try simpler test case:**
   ```bash
   ./bin/emulator  # Test 8-bit VM first
   ```

4. **Review error messages carefully** - they often indicate the exact problem

---

## Success Criteria

You'll know everything is working when:

✅ `make all` completes without errors
✅ `./bin/launcher` shows menu with 5 options
✅ `./ubuntu-quick.sh` opens QEMU window
✅ Ubuntu boots and shows installer/desktop
✅ You can move mouse and type in QEMU window

Good luck! 🚀
