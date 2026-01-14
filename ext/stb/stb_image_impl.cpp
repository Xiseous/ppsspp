// stb_image implementation file for PPSSPP
// This creates the implementation in a single translation unit

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO // We use memory loading, don't need file I/O
#define STBI_NO_HDR   // No HDR support needed
#define STBI_NO_PIC   // No PIC support needed
#define STBI_NO_PNM   // No PNM support needed
#define STBI_NO_GIF   // No GIF support needed for now (can add later if needed)
#include "stb_image.h"
