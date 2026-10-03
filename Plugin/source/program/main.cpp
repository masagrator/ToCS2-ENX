/*
 * ToCS2-ENX translation plugin (Trails of Cold Steel II Kai, Switch).
 * Port of the former Skyline plugin (Tfoaf/main_patch.cpp) to exlaunch.
 */

#include "lib.hpp"
#include "tocs/common.hpp"
#include "tocs/hidden_menu.hpp"
#include "tocs/text_rendering.hpp"

/* The first file the game opens is used to load the plugin settings from the SD card. */
HOOK_DEFINE_TRAMPOLINE(FsOpenFile) {
    static Result Callback(nn::fs::FileHandle* handle, const char* filepath, int mode) {
        static bool initialized = false;
        if (!initialized) {
            nn::fs::MountSdCardForDebug("sd");
            nn::fs::FileHandle save;
            long filesize = 0;
            if (!Orig(&save, SAVE_PATH, nn::fs::OpenMode_Read)) {
                nn::fs::GetFileSize(&filesize, save);
                char magic_check[5] = "";
                nn::fs::ReadFile(save, 0, &magic_check, 4);
                if (strcmp(magic, magic_check) == 0 && filesize > 4)
                    nn::fs::ReadFile(save, 4, &Settings, std::min<long>(filesize - 4, sizeof(Settings)));
                nn::fs::CloseFile(save);
            }
            else {
                nn::fs::CreateDirectory("sd:/config");
                nn::fs::CreateDirectory(SAVE_DIR);
                nn::fs::CreateFile(SAVE_PATH, 4 + sizeof(Settings));
            }
            SetGpuBoost(Settings.GPUBoost != 0);
            initialized = true;
        }
        return Orig(handle, filepath, mode);
    }
};

/* While the settings menu is open, the game only sees R held (so the menu stays up) and no stick input. */
template<typename State, State* Own>
struct BlockedPad {
    static void Apply(State* KeyState) {
        if (BlockButtons && KeyState != Own) {
            KeyState->Buttons = nn::hid::KEY_R;
            KeyState->LStickY = 0;
        }
    }
};

HOOK_DEFINE_TRAMPOLINE(GetNpadStatePro) {
    static void Callback(nn::hid::NpadFullKeyState* KeyState, u32 const& NpadID) {
        Orig(KeyState, NpadID);
        BlockedPad<nn::hid::NpadFullKeyState, &out3>::Apply(KeyState);
    }
};

HOOK_DEFINE_TRAMPOLINE(GetNpadStateJoyCons) {
    static void Callback(nn::hid::NpadJoyDualState* KeyState, u32 const& NpadID) {
        Orig(KeyState, NpadID);
        BlockedPad<nn::hid::NpadJoyDualState, &out2>::Apply(KeyState);
    }
};

HOOK_DEFINE_TRAMPOLINE(GetNpadStateHandheld) {
    static void Callback(nn::hid::NpadHandheldState* KeyState, u32 const& NpadID) {
        Orig(KeyState, NpadID);
        BlockedPad<nn::hid::NpadHandheldState, &out>::Apply(KeyState);
    }
};

extern "C" void exl_main(void* x0, void* x1) {
    exl::hook::Initialize();

    NSO_main_start = exl::util::modules::GetTargetStart();

    //Hook Function calling UI Text Render
    RenderText::InstallAtOffset(0x47A400);
    RenderText2::InstallAtOffset(0x3015A0);
    RenderText3::InstallAtOffset(0x1968D0);
    RenderText4::InstallAtOffset(0x3519D0);
    RenderTextFromAtlas::InstallAtOffset(0x4867E0);

    //Hook UI Text Render
    SetUIText::InstallAtOffset(0x300A90);

    //Hook Text Width Calcs
    GetTextWidth::InstallAtOffset(0x260230);
    GetTextWidth2::InstallAtOffset(0x260EB0);

    //Hook Text Kerning
    SetTextKerning::InstallAtOffset(0x25FD00);

    //Hook vsnprintf wrapper
    VsnprintfWrapper::InstallAtOffset(0x5CA00);

    //Hook strncat
    Strncat::InstallAtOffset(0x83D6F0);

    //Hook FS
    FsOpenFile::InstallAtFuncPtr(&nn::fs::OpenFile);

    //Hook FPS lock
    FPSlock::InstallAtOffset(0x5D100);

    //Hook buttons
    GetNpadStatePro::InstallAtFuncPtr(static_cast<void (*)(nn::hid::NpadFullKeyState*, u32 const&)>(&nn::hid::GetNpadState));
    GetNpadStateJoyCons::InstallAtFuncPtr(static_cast<void (*)(nn::hid::NpadJoyDualState*, u32 const&)>(&nn::hid::GetNpadState));
    GetNpadStateHandheld::InstallAtFuncPtr(static_cast<void (*)(nn::hid::NpadHandheldState*, u32 const&)>(&nn::hid::GetNpadState));

    //Hook 3D world rendering res
    RenderingRes::InstallAtOffset(0x2E5260);

    //Force game to use nvnSamplerBuilderSetMaxAnisotropy
    exl::patch::CodePatcher(0x78EEDC).Write<u32>(0xD503201F); //nop
    //Force nvnSamplerBuilderSetMaxAnisotropy to set anisotropy to 16.0
    exl::patch::CodePatcher(0x78EEE4).Write<u32>(0x1E261000); //fmov s0, #16.0
}

extern "C" NORETURN void exl_exception_entry() {
    /* Only used for applets/sysmodules. */
    EXL_ABORT("Default exception handler called!");
}
