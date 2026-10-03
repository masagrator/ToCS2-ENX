#pragma once

/* Settings menu (hold R on the title screen) and the hooks it controls. Port of Tfoaf/Hiddenmenu.hpp. */

#include "common.hpp"

inline int8_t indicator = 0;
inline uint8_t options_count = 3;

struct Resolution { uint32_t width, height; const char* label; };

/* Index 4 is the game's default. */
inline Resolution GetResolution(int setting) {
    switch (setting) {
        case 0:  return {640, 360, "[640x360]"};
        case 1:  return {853, 480, "[853x480]"};
        case 2:  return {960, 540, "[960x540]"};
        case 3:  return {1120, 630, "[1120x630]"};
        case 5:  return {1440, 810, "[1440x810]"};
        case 6:  return {1600, 900, "[1600x900]"};
        case 7:  return {1760, 990, "[1760x990]"};
        case 8:  return {1920, 1080, "[1920x1080]"};
        default: return {1280, 720, "[1280x720] (Default)"};
    }
}

HOOK_DEFINE_TRAMPOLINE(RenderingRes) {
    static uint64_t Callback(void* unk, uint32_t width, uint32_t height) {
        Resolution r = GetResolution(Settings.RenderingRes);
        return Orig(unk, r.width, r.height);
    }
};

HOOK_DEFINE_TRAMPOLINE(FPSlock) {
    static uint64_t Callback(void* unk, uint32_t FPStarget) {
        static void* unk_holder = 0;
        if (unk != nullptr)
            unk_holder = unk;
        else
            unk = unk_holder;
        if (Settings.FPS == 30)
            return Orig(unk, FPStarget);
        else
            return Orig(unk, Settings.FPS);
    }
};

inline void MenuText(void* x0, int x, int y, const char* text) {
    SetUIText::Orig(x0, x, y, text, (int)0xFFFFFFFF, 0x00000000, 0x00000000, 0, 0, 1.0, 32.0, 1.0);
}

inline void RenderHiddenMenu(void* x0) {
    SetUIText::Orig(x0, -150, -100, &"█"[0], (int)0xFF000000, 0x00000000, 0x00000000, 0, 0, 1.0, 768.0, 2.0);

    int base_Y = 80;

    MenuText(x0, 80, base_Y+(32*0), "FPS:");
    if (Settings.FPS == 30) {
        MenuText(x0, 260, base_Y+(32*0), "[30]");
        MenuText(x0, 460, base_Y+(32*0), "60");
    }
    else {
        MenuText(x0, 260, base_Y+(32*0), "30");
        MenuText(x0, 460, base_Y+(32*0), "[60]");
    }
    MenuText(x0, 80, base_Y+(32*1), "Rendering Resolution:");
    MenuText(x0, 460, base_Y+(32*1), GetResolution(Settings.RenderingRes).label);

    MenuText(x0, 80, base_Y+(32*2), "Handheld GPU Boost:");
    if (Settings.GPUBoost == 0) {
        MenuText(x0, 460, base_Y+(32*2), "[Off]");
        MenuText(x0, 660, base_Y+(32*2), "On");
    }
    else {
        MenuText(x0, 460, base_Y+(32*2), "Off");
        MenuText(x0, 660, base_Y+(32*2), "[On]");
    }

    MenuText(x0, 60, base_Y+(32*indicator), ">");

    if (indicator == 1)
        MenuText(x0, 320, base_Y+(32*11), "To apply this setting you must restart game.");

    ReadPads();

    static bool DDOWN_buttonHeld = false;
    static bool DUP_buttonHeld = false;
    static bool A_buttonHeld = false;

    if (AnyPadHeld(nn::hid::KEY_DDOWN)) {
        if (!DDOWN_buttonHeld) {
            DDOWN_buttonHeld = true;
            if (indicator+1 < options_count) indicator++;
        }
    }
    else DDOWN_buttonHeld = false;

    if (AnyPadHeld(nn::hid::KEY_DUP)) {
        if (!DUP_buttonHeld) {
            DUP_buttonHeld = true;
            if (indicator-1 >= 0) indicator--;
        }
    }
    else DUP_buttonHeld = false;

    if (AnyPadHeld(nn::hid::KEY_A)) {
        if (!A_buttonHeld) {
            A_buttonHeld = true;
            switch(indicator) {
                case 0:
                    if (Settings.FPS == 60) Settings.FPS = 30;
                    else Settings.FPS = 60;
                    FPSlock::Callback(nullptr, Settings.FPS);
                    break;
                case 1:
                    if (Settings.RenderingRes < 0) Settings.RenderingRes = 4;
                    else if (Settings.RenderingRes < 8) Settings.RenderingRes += 1;
                    else Settings.RenderingRes = 0;
                    break;
                case 2:
                    Settings.GPUBoost = Settings.GPUBoost == 0 ? 1 : 0;
                    SetGpuBoost(Settings.GPUBoost != 0);
                    break;
            }
        }
    }
    else A_buttonHeld = false;
}
