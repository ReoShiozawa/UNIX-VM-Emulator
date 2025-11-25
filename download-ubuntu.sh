#!/bin/bash
# Download and verify Ubuntu ISO

set -e

UBUNTU_VERSION="24.04.3"
UBUNTU_VARIANT="desktop"
ARCH="amd64"
ISO_NAME="ubuntu-${UBUNTU_VERSION}-${UBUNTU_VARIANT}-${ARCH}.iso"
ISO_URL="https://releases.ubuntu.com/noble/ubuntu-24.04.3-desktop-amd64.iso"
ISO_DIR="./isos"
ISO_PATH="${ISO_DIR}/${ISO_NAME}"

# Create ISO directory if it doesn't exist
mkdir -p "${ISO_DIR}"

# Check if ISO already exists
if [ -f "${ISO_PATH}" ]; then
    echo "✓ ISO already exists: ${ISO_PATH}"
    ls -lh "${ISO_PATH}"
    exit 0
fi

echo "╔════════════════════════════════════════════╗"
echo "║  Ubuntu ISO Downloader                    ║"
echo "╚════════════════════════════════════════════╝"
echo ""
echo "Downloading: ${ISO_NAME}"
echo "Version: Ubuntu ${UBUNTU_VERSION} ${UBUNTU_VARIANT} ${ARCH}"
echo "Size: ~4.5 GB (this may take a few minutes)"
echo ""
echo "From: ${ISO_URL}"
echo "To:   ${ISO_PATH}"
echo ""

# Download with progress bar
if command -v curl &> /dev/null; then
    echo "Using curl..."
    curl -L -C - --progress-bar "${ISO_URL}" -o "${ISO_PATH}"
elif command -v wget &> /dev/null; then
    echo "Using wget..."
    wget -c --show-progress "${ISO_URL}" -O "${ISO_PATH}"
else
    echo "Error: Neither curl nor wget found"
    exit 1
fi

echo ""
if [ -f "${ISO_PATH}" ]; then
    SIZE=$(du -h "${ISO_PATH}" | cut -f1)
    echo "✓ Download complete!"
    echo "File: ${ISO_PATH} (${SIZE})"
    echo ""
    echo "You can now launch Ubuntu with:"
    echo "  ./run-ubuntu.sh ${ISO_PATH}"
else
    echo "✗ Download failed"
    exit 1
fi
