#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build-linux"
BUILD_TYPE="${1:-Release}"

echo "=== Mollusk Voice Changer - Linux Build ==="
echo "Build type: $BUILD_TYPE"
echo "Build dir:  $BUILD_DIR"

# ----------------------------------------------------------------
# Install dependencies
# ----------------------------------------------------------------
install_deps() {
    echo "--- Installing dependencies ---"

    if command -v apt-get &> /dev/null; then
        # Debian / Ubuntu
        sudo apt-get update -qq
        sudo apt-get install -y \
            cmake ninja-build g++ pkg-config \
            libasound2-dev libjack-jackd2-dev \
            libfreetype-dev libfontconfig1-dev \
            libx11-dev libxinerama-dev libxrandr-dev \
            libxcursor-dev libxcomposite-dev libxext-dev \
            libglib2.0-dev mesa-common-dev libglu1-mesa-dev

    elif command -v dnf &> /dev/null; then
        # Fedora / RHEL / CentOS
        sudo dnf install -y \
            cmake ninja-build gcc-c++ pkgconfig \
            alsa-lib-devel jack-audio-connection-kit-devel \
            freetype-devel fontconfig-devel \
            libX11-devel libXinerama-devel libXrandr-devel \
            libXcursor-devel libXcomposite-devel libXext-devel \
            glib2-devel mesa-libGL-devel mesa-libGLU-devel

    elif command -v pacman &> /dev/null; then
        # Arch Linux / Manjaro
        sudo pacman -Sy --noconfirm \
            cmake ninja gcc pkgconf \
            alsa-lib jack2 \
            freetype2 fontconfig \
            libx11 libxinerama libxrandr \
            libxcursor libxcomposite libxext \
            glib2 mesa glu

    elif command -v zypper &> /dev/null; then
        # openSUSE
        sudo zypper install -y \
            cmake ninja gcc-c++ pkg-config \
            alsa-devel libjack-devel \
            freetype2-devel fontconfig-devel \
            libX11-devel libXinerama-devel libXrandr-devel \
            libXcursor-devel libXcomposite-devel libXext-devel \
            glib2-devel Mesa-devel glu-devel

    else
        echo "ERROR: Unsupported package manager. Install dependencies manually." >&2
        echo "Required: cmake, ninja, g++, alsa-dev, jack-dev, freetype-dev," >&2
        echo "  fontconfig-dev, x11-dev, xinerama-dev, xrandr-dev, xcursor-dev," >&2
        echo "  xcomposite-dev, xext-dev, glib2-dev, gl-dev, glu-dev" >&2
        exit 1
    fi
}

install_deps

# ----------------------------------------------------------------
# Configure
# ----------------------------------------------------------------
echo "--- Configuring ---"
cmake -S "$PROJECT_DIR" -B "$BUILD_DIR" \
    -G Ninja \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE"

# ----------------------------------------------------------------
# Build
# ----------------------------------------------------------------
echo "--- Building ---"
cmake --build "$BUILD_DIR" --config "$BUILD_TYPE" --parallel "$(nproc)"

echo ""
echo "=== Build complete ==="
echo "Artifacts:"
find "$BUILD_DIR" -name "MolluskVoiceChanger" -type f 2>/dev/null | grep -v ".cmake" | sed 's/^/  /'
find "$BUILD_DIR" -name "*.vst3" -type d 2>/dev/null | sed 's/^/  /'
