#!/bin/bash
# build_switch.sh - Build PPSSPP for Nintendo Switch
# 
# Prerequisites:
#   - devkitPro installed (https://devkitpro.org/wiki/Getting_Started)
#   - Switch development packages installed via dkp-pacman
#
# Install dependencies with:
#   dkp-pacman -S switch-dev switch-sdl2 switch-sdl2_ttf switch-mesa switch-glad \
#               switch-glm switch-libdrm_nouveau switch-zlib switch-bzip2 \
#               switch-libpng switch-freetype switch-libjpeg-turbo libnx

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== PPSSPP Switch Build Script ===${NC}"

# Check for devkitPro
if [ -z "$DEVKITPRO" ]; then
    if [ -d "/opt/devkitpro" ]; then
        export DEVKITPRO=/opt/devkitpro
    else
        echo -e "${RED}Error: DEVKITPRO environment variable not set${NC}"
        echo "Please install devkitPro and source the environment:"
        echo "  source /opt/devkitpro/switchvars.sh"
        exit 1
    fi
fi

# Source Switch environment variables
if [ -f "$DEVKITPRO/switchvars.sh" ]; then
    echo -e "${YELLOW}Sourcing Switch environment...${NC}"
    source "$DEVKITPRO/switchvars.sh"
else
    echo -e "${RED}Error: switchvars.sh not found${NC}"
    exit 1
fi

# Check for required tools
if ! command -v aarch64-none-elf-gcc &> /dev/null; then
    echo -e "${RED}Error: aarch64-none-elf-gcc not found${NC}"
    echo "Please install switch-dev: dkp-pacman -S switch-dev"
    exit 1
fi

# Get script directory (where PPSSPP source is)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Build directory
BUILD_DIR="build-switch"

# Parse arguments
CLEAN_BUILD=false
JOBS=$(nproc 2>/dev/null || echo 4)

while [[ $# -gt 0 ]]; do
    case $1 in
        --clean)
            CLEAN_BUILD=true
            shift
            ;;
        -j*)
            JOBS="${1#-j}"
            shift
            ;;
        --help|-h)
            echo "Usage: $0 [options]"
            echo "Options:"
            echo "  --clean    Clean build directory before building"
            echo "  -jN        Use N parallel jobs (default: $(nproc))"
            echo "  --help     Show this help message"
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            exit 1
            ;;
    esac
done

# Clean if requested
if [ "$CLEAN_BUILD" = true ]; then
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    rm -rf "$BUILD_DIR"
fi

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Generate git version
echo -e "${YELLOW}Generating version info...${NC}"
GIT_VERSION=$(git describe --always --tags 2>/dev/null || echo "dev")
echo "const char *PPSSPP_GIT_VERSION = \"$GIT_VERSION\";" > ../git-version.cpp
echo "#define PPSSPP_GIT_VERSION_NO_UPDATE 1" >> ../git-version.cpp
echo -e "Version: ${GREEN}$GIT_VERSION${NC}"

# Configure with CMake
echo -e "${YELLOW}Configuring CMake...${NC}"
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE="$DEVKITPRO/cmake/Switch.cmake" \
    -DCMAKE_BUILD_TYPE=Release \
    -DUSE_LIBNX=ON \
    -DUSING_GLES2=ON \
    -DMOBILE_DEVICE=ON \
    -DUSE_FFMPEG=OFF \
    -DUSE_DISCORD=OFF \
    -DUSE_MINIUPNPC=OFF \
    -DHEADLESS=OFF \
    -DUNITTEST=OFF \
    -DCMAKE_DISABLE_FIND_PACKAGE_SDL2_ttf=ON \
    -DCMAKE_DISABLE_FIND_PACKAGE_Fontconfig=ON

# Build
echo -e "${YELLOW}Building with $JOBS parallel jobs...${NC}"
make -j"$JOBS"

# Create NRO
echo -e "${YELLOW}Creating NRO file...${NC}"
if [ -f PPSSPPSDL ]; then
    nacptool --create "PPSSPP" "PPSSPP Team" "$GIT_VERSION" ppsspp.nacp 2>/dev/null || true
    
    # Try with icon first, fall back without
    if [ -f ../icons/icon-114.png ]; then
        elf2nro PPSSPPSDL PPSSPPSDL.nro --icon=../icons/icon-114.png --nacp=ppsspp.nacp 2>/dev/null || \
        elf2nro PPSSPPSDL PPSSPPSDL.nro --nacp=ppsspp.nacp 2>/dev/null || \
        elf2nro PPSSPPSDL PPSSPPSDL.nro
    else
        elf2nro PPSSPPSDL PPSSPPSDL.nro --nacp=ppsspp.nacp 2>/dev/null || \
        elf2nro PPSSPPSDL PPSSPPSDL.nro
    fi
    
    if [ -f PPSSPPSDL.nro ]; then
        echo -e "${GREEN}Successfully created PPSSPPSDL.nro${NC}"
    else
        echo -e "${YELLOW}Note: NRO creation may have issues, but ELF was built${NC}"
    fi
else
    echo -e "${RED}Error: PPSSPPSDL executable not found${NC}"
    exit 1
fi

# Create output package
echo -e "${YELLOW}Packaging...${NC}"
OUTPUT_DIR="$SCRIPT_DIR/ppsspp-switch"
rm -rf "$OUTPUT_DIR"
mkdir -p "$OUTPUT_DIR/switch/ppsspp"

if [ -f PPSSPPSDL.nro ]; then
    cp PPSSPPSDL.nro "$OUTPUT_DIR/switch/ppsspp/PPSSPP.nro"
elif [ -f PPSSPPSDL ]; then
    cp PPSSPPSDL "$OUTPUT_DIR/switch/ppsspp/PPSSPP.nro"
fi

cp -r ../assets "$OUTPUT_DIR/switch/ppsspp/"

echo ""
echo -e "${GREEN}=== Build Complete ===${NC}"
echo -e "Output directory: ${YELLOW}$OUTPUT_DIR${NC}"
echo ""
echo "To install on Switch:"
echo "  1. Copy the contents of 'ppsspp-switch' to your SD card root"
echo "  2. Launch from Homebrew Menu"
echo ""
