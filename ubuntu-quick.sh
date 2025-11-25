#!/bin/bash
# Quick Ubuntu QEMU Launcher - Simplified

ISO_FILE="${1:-./isos/ubuntu-24.04.3-desktop-amd64.iso}"
DISK_DIR="./qemu-images"
DISK_FILE="${DISK_DIR}/ubuntu-disk.qcow2"

# Kill any existing QEMU processes
pkill -9 qemu-system-x86_64 2>/dev/null
sleep 1

# Check prerequisites
if ! command -v qemu-system-x86_64 &> /dev/null; then
    echo "QEMU not installed: brew install qemu"
    exit 1
fi

if [ ! -f "${ISO_FILE}" ]; then
    echo "ISO not found: ${ISO_FILE}"
    exit 1
fi

# Setup disk
mkdir -p "${DISK_DIR}"
# Always create fresh disk
rm -f "${DISK_FILE}"
echo "Creating virtual disk..."
qemu-img create -f qcow2 "${DISK_FILE}" 50G 2>&1 | grep -v "^$"

# Calculate memory
TOTAL_RAM=$(sysctl -n hw.memsize 2>/dev/null | awk '{print int($1/1024/1024/1024)}' || echo "4")
RAM=$((TOTAL_RAM / 2))
[ $RAM -lt 2 ] && RAM=2
[ $RAM -gt 8 ] && RAM=8

echo "Starting Ubuntu... (${RAM}GB RAM)"

# Launch QEMU with proper locking
qemu-system-x86_64 \
    -machine type=q35 \
    -m ${RAM}G \
    -smp cores=4,threads=1 \
    -cdrom "${ISO_FILE}" \
    -drive file="${DISK_FILE}",format=qcow2,if=virtio,cache=writeback \
    -netdev user,id=net0,hostfwd=tcp::2222-:22 \
    -device e1000,netdev=net0 \
    -display cocoa \
    -vga virtio \
    -pidfile "/tmp/qemu-ubuntu.pid"

# Cleanup on exit
rm -f "/tmp/qemu-ubuntu.pid"

