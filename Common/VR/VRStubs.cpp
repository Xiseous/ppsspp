// Stub implementations for VR functions when OPENXR is not enabled
// This file should only be compiled when OPENXR is OFF

#include "PPSSPPVR.h"
#include <cstddef>

// VR app flow integration
bool IsVREnabled() { return false; }
void InitVROnAndroid(void *vm, void *activity, const char *system, int version,
                     const char *name) {}
void EnterVR(bool firstStart) {}
void GetVRResolutionPerEye(int *width, int *height) {}
void SetVRCallbacks(void (*axis)(const AxisInput *axis, size_t count),
                    bool (*key)(const KeyInput &key),
                    void (*touch)(const TouchInput &touch)) {}

// VR input integration
void SetVRAppMode(VRAppMode mode) {}
void UpdateVRInput(bool haptics, float dp_xscale, float dp_yscale) {}
bool UpdateVRAxis(const AxisInput *axes, size_t count) { return true; }
bool UpdateVRKeys(const KeyInput &key) { return true; }

// VR games compatibility
void PreprocessStepVR(void *step) {}
void SetVRCompat(VRCompatFlag flag, long value) {}

// VR rendering integration
void *BindVRFramebuffer() { return nullptr; }
bool StartVRRender() { return false; }
void FinishVRRender() {}
void PreVRFrameRender(int fboIndex) {}
void PostVRFrameRender() {}
int GetVRFBOIndex() { return 0; }
int GetVRPassesCount() { return 1; }
bool IsPassthroughSupported() { return false; }
bool IsBigScreenVRMode() { return true; }
bool IsFlatVRGame() { return false; }
bool IsFlatVRScene() { return true; }
bool IsGameVRScene() { return false; }
bool IsImmersiveVRMode() { return false; }
bool Is2DVRObject(float *projMatrix, bool ortho) { return true; }
void UpdateVRParams(float *projMatrix) {}
void UpdateVRProjection(float *projMatrix, float *output) {
  for (int i = 0; i < 16; i++) {
    output[i] = projMatrix[i];
  }
}
void UpdateVRView(float *leftEye, float *rightEye) {}
void UpdateVRViewMatrices() {}
