#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build-linux"
BUILD_TYPE="${1:-Release}"

echo "=== Mollusk Voice Changer - Linux Build ==="
echo "Build type: $BUILD_TYPE"
echo "Build dir:  $BUILD_DIR"

# Install dependencies
install_deps() {
    echo "--- Installing dependencies ---"
    sudo apt-get update -qq
    sudo apt-get install -y \
        cmake \
        ninja-build \
        g++ \
        libasound2-dev \
        libjack-jackd2-dev \
        libfreetype-dev \
        libfontconfig1-dev \
        libx11-dev \
        libxinerama-dev \
        libxrandr-dev \
        libxcursor-dev \
        libxcomposite-dev \
        libxext-dev \
        libglib2.0-dev \
        mesa-common-dev \
        libglu1-mesa-dev \
        pkg-config
}

install_deps

# Configure
echo "--- Configuring ---"
cmake -S "$PROJECT_DIR" -B "$BUILD_DIR" \
    -G Ninja \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE"

# Build
echo "--- Building ---"
cmake --build "$BUILD_DIR" --config "$BUILD_TYPE" --parallel "$(nproc)"

echo ""
echo "=== Build complete ==="
echo "Artifacts:"
find "$BUILD_DIR" -name "MolluskVoiceChanger" -type f 2>/dev/null | grep -v ".cmake" | sed 's/^/  /'
find "$BUILD_DIR" -name "*.vst3" -type d 2>/dev/null | sed 's/^/  /'
