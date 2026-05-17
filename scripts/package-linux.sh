#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build-linux"
RELEASE_DIR="$PROJECT_DIR/release/linux"
TOOLS_DIR="$PROJECT_DIR/.appimage-tools"

VERSION=$(grep -m1 "^project(" "$PROJECT_DIR/CMakeLists.txt" | grep -oP 'VERSION \K[0-9.]+')
BINARY="$BUILD_DIR/MolluskVoiceChanger_artefacts/Release/Mollusk Voice Changer"
VST3_DIR="$BUILD_DIR/MolluskVoiceChangerPlugin_artefacts/Release/VST3/Mollusk Voice Changer.vst3"

echo "=== Mollusk Voice Changer - Linux Packaging ==="
echo "Version: $VERSION"
echo "Output:  $RELEASE_DIR"

if [ ! -f "$BINARY" ]; then
    echo "ERROR: Build not found. Run scripts/build-linux.sh first." >&2
    exit 1
fi

mkdir -p "$RELEASE_DIR" "$TOOLS_DIR"

# ----------------------------------------------------------------
# Download linuxdeploy tools
# ----------------------------------------------------------------
LINUXDEPLOY="$TOOLS_DIR/linuxdeploy-x86_64.AppImage"
LINUXDEPLOY_PLUGIN="$TOOLS_DIR/linuxdeploy-plugin-appimage-x86_64.AppImage"

if [ ! -f "$LINUXDEPLOY" ]; then
    echo "--- Downloading linuxdeploy ---"
    curl -fsSL "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage" \
        -o "$LINUXDEPLOY"
    chmod +x "$LINUXDEPLOY"
fi

if [ ! -f "$LINUXDEPLOY_PLUGIN" ]; then
    echo "--- Downloading linuxdeploy-plugin-appimage ---"
    curl -fsSL "https://github.com/linuxdeploy/linuxdeploy-plugin-appimage/releases/download/continuous/linuxdeploy-plugin-appimage-x86_64.AppImage" \
        -o "$LINUXDEPLOY_PLUGIN"
    chmod +x "$LINUXDEPLOY_PLUGIN"
fi

export PATH="$TOOLS_DIR:$PATH"
export APPIMAGE_EXTRACT_AND_RUN=1

# ----------------------------------------------------------------
# AppImage
# ----------------------------------------------------------------
echo "--- Building AppImage ---"

APPDIR="$PROJECT_DIR/.appimage-build/AppDir"
rm -rf "$PROJECT_DIR/.appimage-build"
mkdir -p "$APPDIR/usr/bin"
mkdir -p "$APPDIR/usr/share/applications"
mkdir -p "$APPDIR/usr/share/icons/hicolor/256x256/apps"

cp "$BINARY" "$APPDIR/usr/bin/mollusk-voice-changer"
cp "$PROJECT_DIR/assets/icons/app_icon.png" \
    "$APPDIR/usr/share/icons/hicolor/256x256/apps/mollusk-voice-changer.png"

cat > "$APPDIR/usr/share/applications/mollusk-voice-changer.desktop" <<EOF
[Desktop Entry]
Type=Application
Name=Mollusk Voice Changer
Exec=mollusk-voice-changer
Icon=mollusk-voice-changer
Comment=Voice changer with character presets
Categories=Audio;AudioVideo;
EOF

APPIMAGE_OUT="$RELEASE_DIR/MolluskVoiceChanger-${VERSION}-x86_64.AppImage"

cd "$PROJECT_DIR/.appimage-build"
"$LINUXDEPLOY" \
    --appdir "$APPDIR" \
    --output appimage

find "$PROJECT_DIR/.appimage-build" -name "*.AppImage" -exec mv {} "$APPIMAGE_OUT" \;
cd "$PROJECT_DIR"
rm -rf "$PROJECT_DIR/.appimage-build"

echo "  -> $APPIMAGE_OUT"

# ----------------------------------------------------------------
# VST3
# ----------------------------------------------------------------
echo "--- Packaging VST3 ---"

VST3_OUT="$RELEASE_DIR/MolluskVoiceChanger-${VERSION}-linux-vst3.tar.gz"
tar -czf "$VST3_OUT" \
    -C "$(dirname "$VST3_DIR")" \
    "$(basename "$VST3_DIR")"

echo "  -> $VST3_OUT"

echo ""
echo "=== Packaging complete ==="
ls -lh "$RELEASE_DIR/"
