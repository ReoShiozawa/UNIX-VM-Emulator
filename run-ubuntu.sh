#!/bin/bash
# QEMU Ubuntu Launcher - macOS Optimized

ISO_FILE="${1:-./isos/ubuntu-24.04.3-desktop-amd64.iso}"
DISK_DIR="./qemu-images"
DISK_FILE="${DISK_DIR}/ubuntu-disk.qcow2"

# Kill any existing QEMU processes to avoid lock conflicts
pkill -9 qemu-system-x86_64 2>/dev/null
sleep 1

echo "╔════════════════════════════════════════════╗"
echo "║  Ubuntu QEMU Launcher                     ║"
echo "╚════════════════════════════════════════════╝"
echo ""

# Check QEMU
if ! command -v qemu-system-x86_64 &> /dev/null; then
    echo "❌ QEMU not installed. Install with:"
    echo "   brew install qemu"
    exit 1
fi

echo "✓ QEMU: $(qemu-system-x86_64 --version | head -1)"

# Check ISO
if [ ! -f "${ISO_FILE}" ]; then
    echo "❌ ISO not found: ${ISO_FILE}"
    echo "   Download with: ./download-ubuntu.sh"
    exit 1
fi

ISO_SIZE=$(du -h "${ISO_FILE}" | cut -f1)
echo "✓ ISO: ${ISO_FILE} (${ISO_SIZE})"

# Setup disk
mkdir -p "${DISK_DIR}"
# Create fresh disk each time to avoid lock conflicts
rm -f "${DISK_FILE}"
echo "Creating disk: ${DISK_FILE} (this may take a minute...)"
qemu-img create -f qcow2 "${DISK_FILE}" 50G 2>&1 | tail -1
echo "✓ Disk: ${DISK_FILE}"
echo ""

# Get available RAM
TOTAL_RAM=$(sysctl -n hw.memsize 2>/dev/null | awk '{print int($1/1024/1024/1024)}' || echo "4")
RAM=$((TOTAL_RAM / 2))
[ $RAM -lt 2 ] && RAM=2
[ $RAM -gt 8 ] && RAM=8

echo "Launching QEMU with ${RAM}GB RAM..."
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

# macOS: Use Cocoa display, proper disk options
qemu-system-x86_64 \
    -machine type=q35 \
    -m "${RAM}G" \
    -smp cores=4,threads=1 \
    -cdrom "${ISO_FILE}" \
    -drive file="${DISK_FILE}",format=qcow2,if=virtio,cache=writeback \
    -device e1000,netdev=net0 \
    -netdev user,id=net0,hostfwd=tcp::2222-:22 \
    -device intel-hda \
    -device hda-duplex \
    -display cocoa \
    -vga virtio \
    -pidfile "/tmp/qemu-ubuntu.pid"

# Cleanup
rm -f "/tmp/qemu-ubuntu.pid"

echo ""
echo "╔════════════════════════════════════════════╗"
echo "║  Ubuntu session closed                    ║"
echo "╚════════════════════════════════════════════╝"

