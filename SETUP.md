# Setup Instructions - Ubuntu QEMU Integration

## 1. Prerequisites ✓

### macOS
```bash
# Already installed:
✓ GCC (C compiler)
✓ QEMU (x86-64 emulator)

# Optional:
brew install sdl2    # For GUI visualization
```

### Linux (Debian/Ubuntu)
```bash
sudo apt update
sudo apt install build-essential qemu-system-x86 libsdl2-dev
```

## 2. Project Setup ✓

```bash
cd /Users/kinoko/Documents/app/emulator
make clean
make all
```

### Build Output
```
✓ bin/launcher       (Interactive menu)
✓ bin/emulator       (8-bit RISC VM)
✓ bin/vm64          (x86-64 VM)
✓ bin/emulator-gui  (With SDL2 - optional)
✓ bin/imggen        (Image generator)
```

## 3. Download Ubuntu ISO ⏳

Current Status: **Downloading...**

### Option A: Auto-Download
```bash
./download-ubuntu.sh
```
This will:
- Download Ubuntu 24.04.3 desktop
- Place in `./isos/ubuntu-24.04.3-desktop-amd64.iso`
- Take ~5-10 minutes on typical internet

### Option B: Manual Download
```bash
# Download separately if preferred:
curl -o isos/ubuntu-24.04.3-desktop-amd64.iso \
  https://releases.ubuntu.com/noble/ubuntu-24.04.3-desktop-amd64.iso
```

### Option C: Use Existing ISO
```bash
# If you already have an Ubuntu ISO:
./ubuntu-quick.sh /path/to/your/ubuntu.iso
```

## 4. Launch Ubuntu 🚀

Once download completes:

### Quick Launch (Recommended)
```bash
./ubuntu-quick.sh
```

### Via Interactive Menu
```bash
./bin/launcher
# → Select option 5: Ubuntu with QEMU
```

### With Custom Settings
```bash
./run-ubuntu.sh /path/to/ubuntu.iso
```

## 5. First Run

When QEMU launches:

1. **QEMU Window Opens** - This is Ubuntu booting
   - First boot takes ~30 seconds
   - Subsequent boots are faster

2. **Ubuntu Installer Appears** - Choose:
   - Language: English (or your choice)
   - Install Ubuntu (or Try Ubuntu)
   - Keyboard layout
   - Network (auto-detected)
   - Installation type (default fine)
   - Continue through steps

3. **Desktop Ready** - Full Ubuntu environment!
   - Browser, terminal, files, etc.
   - Network works (user-mode NAT)
   - SSH available on localhost:2222

## 6. Using Ubuntu

### Inside QEMU

**Terminal:**
```bash
# Access terminal in Ubuntu
Applications → Terminal

# Or keyboard shortcut: Ctrl+Alt+T
```

**File Sharing:**
- QEMU uses user-mode networking
- Share files via SSH or copy/paste

**Network:**
- Automatically connected via user-mode NAT
- Can access external internet
- No firewall blocking

### From Host Machine

**SSH into Ubuntu:**
```bash
ssh -p 2222 localhost      # From your Mac
# Username: ubuntu
# Password: ubuntu (first run)
```

**File Transfer:**
```bash
scp -P 2222 file.txt localhost:/home/ubuntu/
```

## 7. Stopping Ubuntu

**Method 1: Normal Shutdown**
- Applications → Power Off
- Choose "Power Off"

**Method 2: Force Close**
- Click QEMU window close button
- Or Alt+F4 inside QEMU

**Method 3: Terminal**
```bash
# From another terminal:
killall qemu-system-x86_64
```

## 8. Persistent Storage

The virtual disk `./qemu-images/ubuntu-disk.qcow2`:
- Stores all files you create in Ubuntu
- Persists between sessions
- Grows dynamically (up to 50GB)
- Can be backed up or transferred

## 9. Performance Tips

### Memory Allocation
- Automatically detects your system RAM
- Allocates 50% to QEMU (max 8GB)
- Adjust in `run-ubuntu.sh` if needed

### Disk I/O
- Virtual disk is 50GB qcow2 format
- Uses sparse allocation (grows as needed)
- First write slower than SSD, but acceptable

### CPU Cores
- Defaults to 4 cores
- Adjust in `run-ubuntu.sh`: `-smp cores=2` to change

### Display
- Uses GTK display (native macOS window)
- VGA acceleration enabled
- Reduce resolution in Ubuntu settings if slow

## 10. Troubleshooting

**QEMU crashes or won't start:**
```bash
# Try without KVM acceleration:
# Edit run-ubuntu.sh, remove "-enable-kvm" flag
```

**Out of disk space:**
```bash
# Check available space:
df -h ./qemu-images/

# Compact the virtual disk:
qemu-img convert -O qcow2 ubuntu-disk.qcow2 ubuntu-disk-compact.qcow2
```

**Slow performance:**
```bash
# Allocate more memory:
# Edit run-ubuntu.sh: change -m value (e.g., -m 6G)
```

**Can't connect to internet:**
```bash
# Check network in Ubuntu:
# Settings → Network → Check connection
# May need to choose "Wired Connection" if not auto-detected
```

## 11. System Requirements

| Component | Requirement | Your System |
|-----------|-------------|------------|
| Storage | 4GB ISO + 50GB disk | ✓ |
| RAM | 2GB minimum | ✓ |
| CPU | Multi-core recommended | ✓ |
| Internet | For download only | ✓ |

## 12. Next Steps

After Ubuntu is set up:

1. **Update System**
   ```bash
   sudo apt update && sudo apt upgrade
   ```

2. **Install Tools**
   ```bash
   sudo apt install build-essential git curl wget
   ```

3. **Development**
   - GCC/G++ pre-installed
   - Python 3 pre-installed
   - Node.js available via apt

4. **Servers**
   - SSH: `sudo apt install openssh-server`
   - Web: `sudo apt install nginx`
   - Database: `sudo apt install sqlite3`

## 13. Additional Resources

- **Ubuntu Docs**: https://ubuntu.com/
- **QEMU Docs**: https://www.qemu.org/documentation/
- **Emulator README**: `./README.md`
- **Launcher Guide**: `./LAUNCHER_GUIDE.md`

---

**Download Status**: 12% complete
**ETA**: ~40-50 minutes at typical speeds

Once download finishes, you'll be able to run `./bin/launcher` and select Ubuntu option! 🎉
