// SPDX-FileCopyrightText: 2019-2022 Connor McLaughlin <stenzek@gmail.com>
// SPDX-License-Identifier: (GPL-2.0 OR GPL-3.0 OR CC-BY-NC-ND-4.0)

// Derived from Duckstation's RetroAchievements implementation by stenzek as can
// be seen above, relicensed to GPL 2.0. Modifications and deletions have been
// made where needed. Refactoring it into a more PPSSPP-like style may or may
// not be a good idea - it'll make it harder to copy new achievement features
// from Duckstation.

#pragma once

#include <set>
#include <string>

#ifndef PPSSPP_PLATFORM_SWITCH
#include "ppsspp_config.h"
#endif

#include "Common/CommonTypes.h"
#include "Core/Loaders.h" // for IdentifiedFileType

class Path;
class PointerWrap;
class FileLoader;

struct rc_client_t;

namespace Achievements {

#if !PPSSPP_PLATFORM(SWITCH)

struct Statistics {
  // Debug stats
  int badMemoryAccessCount;
};

// Returns true if the user is logged in properly, and everything is set up for
// playing games with achievements.
bool IsLoggedIn();

// Returns true if in a game, and achievements are active in the current game.
bool IsActive();

// Returns true if unofficial achievements are enabled.
bool UnofficialEnabled();

// Returns true if encore-mode is active.
bool EncoreModeActive();

// Returns true if the emulator should hold off on executing game code, such as
// during game identification.
bool IsBlockingExecution();

// Returns true if features such as save states should be disabled.
// This should only be used for controlling functionality of the following
// things, which are banned in Challenge/Hardcore mode:
//
// * Savestates
// * Slowdown time (though hard to fully prevent, could use crazy post shaders
// or software rendering...)
// * Debugging
// * Cheats
// * and similar...
bool HardcoreModeActive();

// Same as ChallengeModeActive but comes with a convenient user message. Don't
// use for every-frame checks or UI enablement, only for shortcut keys and
// commands. You should look up the message in I18NCat::Achievements. If no
// message is specified, a standard "This feature is not available in Hardcore
// Mode" message will be shown. Also returns true if hardcore mode is active.
// Specify isSaveAction so we can still permit saves (but not loads) in hardcore
// mode if that option is enabled.
bool WarnUserIfHardcoreModeActive(bool isSaveStateAction,
                                  std::string_view message = "");

// Returns the length of the string. If (size_t)-1, there's no message.
size_t GetRichPresenceMessage(char *buffer, size_t bufSize);

// Returns true if the user has unsaved RAIntegration changes. Should prompt the
// user to be sure they want to exit.
bool RAIntegrationDirty();

// The new API is so much nicer that we can use it directly instead of wrapping
// it. So let's expose the client. Will of course return nullptr if not active.
rc_client_t *GetClient();

void Initialize();
void UpdateSettings();

bool LoginProblems(std::string *errorString);
bool HasToken();

/// Called when the system is being shut down. If Shutdown() returns false, the
/// shutdown should be aborted if possible.
bool Shutdown();

void DownloadImageIfMissing(const std::string &cache_key, std::string_view url);

/// Called once a frame at vsync time on the CPU thread, during gameplay.
void FrameUpdate();

/// Called every frame to let background processing happen.
void Idle();

/// Saves/loads state.
void DoState(PointerWrap &p);

/// Returns true if the current game has any achievements or leaderboards.
bool HasAchievementsOrLeaderboards();

bool LoginAsync(const char *username, const char *password);
void Logout();

void SetGame(const Path &path, IdentifiedFileType fileType,
             FileLoader *fileLoader);
void ChangeUMD(const Path &path,
               FileLoader *fileLoader); // for in-game UMD change
void UnloadGame();                      // Call when leaving a game.

Statistics GetStatistics();

std::string GetGameAchievementSummary();

std::set<uint32_t> GetActiveChallengeIDs();

#else

// Stubs for Switch
struct Statistics {
  int badMemoryAccessCount;
};

inline bool IsLoggedIn() { return false; }
inline bool IsActive() { return false; }
inline bool UnofficialEnabled() { return false; }
inline bool EncoreModeActive() { return false; }
inline bool IsBlockingExecution() { return false; }
inline bool HardcoreModeActive() { return false; }
inline bool WarnUserIfHardcoreModeActive(bool isSaveStateAction,
                                         std::string_view message = "") {
  return false;
}
inline size_t GetRichPresenceMessage(char *buffer, size_t bufSize) {
  return (size_t)-1;
}
inline bool RAIntegrationDirty() { return false; }
inline rc_client_t *GetClient() { return nullptr; }
inline void Initialize() {}
inline void UpdateSettings() {}
inline bool LoginProblems(std::string *errorString) { return false; }
inline bool HasToken() { return false; }
inline bool Shutdown() { return true; }
inline void DownloadImageIfMissing(const std::string &cache_key,
                                   std::string_view url) {}
inline void FrameUpdate() {}
inline void Idle() {}
inline void DoState(PointerWrap &p) {}
inline bool HasAchievementsOrLeaderboards() { return false; }
inline bool LoginAsync(const char *username, const char *password) {
  return false;
}
inline void Logout() {}
inline void SetGame(const Path &path, IdentifiedFileType fileType,
                    FileLoader *fileLoader) {}
inline void ChangeUMD(const Path &path, FileLoader *fileLoader) {}
inline void UnloadGame() {}
inline Statistics GetStatistics() { return {0}; }
inline std::string GetGameAchievementSummary() { return ""; }
inline std::set<uint32_t> GetActiveChallengeIDs() {
  return std::set<uint32_t>();
}

#endif

} // namespace Achievements
