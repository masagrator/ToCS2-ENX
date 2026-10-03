#pragma once

/* Text layout fixes for the English text. Port of Tfoaf/TextRendering.hpp. */

#include "common.hpp"
#include "hidden_menu.hpp"

HOOK_DEFINE_TRAMPOLINE(SetTextKerning) {
    static int Callback(void* _this, int kerning) {
        return Orig(_this, 0);
    }
};

HOOK_DEFINE_TRAMPOLINE(GetTextWidth2) {
    static int Callback(void* _this, const char* string, void* width_ptr, void* height_ptr, void* x4, float fontsize, float X_Scale) {
        int width = Orig(_this, string, width_ptr, height_ptr, x4, fontsize, X_Scale);
        temp.text = string;
        temp.width = width;
        return width;
    }
};

HOOK_DEFINE_TRAMPOLINE(GetTextWidth) {
    static int Callback(void* _this, const char* string, void* width_ptr, void* height_ptr, void* x4, float fontsize, float X_Scale) {
        int width = Orig(_this, string, width_ptr, height_ptr, x4, fontsize, X_Scale);
        temp.text = string;
        temp.width = width;
        if ((strncmp("EP ", string, 3) == 0) || (strncmp("CP ", string, 3) == 0)) {
            if (temp.text.length() == 6) {
                std::string string_aft = temp.text.substr(3, 3);
                string_aft.erase(std::remove(string_aft.begin(), string_aft.end(), ' '), string_aft.end());
                CPEP_width = Orig(_this, string_aft.c_str(), width_ptr, height_ptr, x4, fontsize, X_Scale);
            }
        }
        return width;
    }
};

inline void get_UITextcase(uintptr_t LR) {
    ptrdiff_t offsetItr = returnInstructionOffset(LR);
    switch(offsetItr) {
        case 0x4E246C:
            UITextcase = 1;
            break;

        case 0x475D4C:
            UITextcase = 2;
            break;

        case 0x192F20:
            UITextcase = 3;
            break;

        case 0x192F9C:
        case 0x192F84:
            UITextcase = 4;
            break;

        case 0x4F1F34:
            UITextcase = 5;
            break;

        case 0x3063DC:
        case 0x2C6258:
        case 0x2C69FC:
        case 0x2C6C24:
        case 0x2C6448:
        case 0x2C5A60:
        case 0x2C5DB8:
        case 0x2C5B74:
        case 0x2C6B10:
        case 0x4F48FC:
            UITextcase = 6;
            break;

        case 0x351EB0:
            UITextcase = 7;
            break;

        case 0x3510F4:
            UITextcase = 8;
            break;

        case 0x4CCA68:
            UITextcase = 9;
            break;

        case 0x19757C:
            UITextcase = 10;
            break;

        case 0x364498:
            UITextcase = 11;
            break;

        case 0x4EB144:
        case 0x4EA6A0:
            UITextcase = 12;
            break;

        case 0x47A76C:
        case 0x47AA6C:
            UITextcase = 13;
            break;

        default:
            UITextcase = 0;
    }
}

/* strncat: the length argument is passed through untouched. */
HOOK_DEFINE_TRAMPOLINE(Strncat) {
    static char* Callback(char* destination, const char* source, size_t num) {
        ptrdiff_t offsetItr = returnInstructionOffset((uintptr_t)__builtin_return_address(0));
        switch(offsetItr) {
            //Fix printing items descriptions
            case 0x32C200:
            case 0x329214:
                if (strncmp(source, "]\n  [", 5) == 0) {
                    source = source + 4;
                    memset(destination, 0, sizeof(size_t));
                }
                break;
            //Fix printing quartz descriptions
            case 0x328A88:
                if ((strncmp(source, "[#", 2) == 0) && (strncmp(destination, "[#", 2) == 0)) {
                    memset(destination, 0, sizeof(size_t));
                }
                break;
        }
        return Orig(destination, source, num);
    }
};

HOOK_DEFINE_TRAMPOLINE(VsnprintfWrapper) {
    static uint64_t Callback(char* s, size_t n, const char* format, const char* first, const char* second, const char* x5, const char* x6, const char* x7, long double q0, long double q1, long double q2, long double q3, void* x8) {
        ptrdiff_t offsetItr = returnInstructionOffset((uintptr_t)__builtin_return_address(0));
        //Fix printing Talk to button
        if (offsetItr == 0x2C6188) {
            if (strcmp(second, "Talk to") == 0)
                return Orig(s, n, format, "Talk to ", first, x5, x6, x7, q0, q1, q2, q3, x8);
        }
        return Orig(s, n, format, first, second, x5, x6, x7, q0, q1, q2, q3, x8);
    }
};

/* These only record which UI is being drawn, based on who called them. */
HOOK_DEFINE_TRAMPOLINE(RenderText) {
    static uint64_t Callback(void* _this) {
        get_UITextcase((uintptr_t)__builtin_return_address(0));
        return Orig(_this);
    }
};

HOOK_DEFINE_TRAMPOLINE(RenderText3) {
    static uint64_t Callback(void* _this) {
        get_UITextcase((uintptr_t)__builtin_return_address(0));
        return Orig(_this);
    }
};

HOOK_DEFINE_TRAMPOLINE(RenderText4) {
    static uint64_t Callback(void* _this) {
        get_UITextcase((uintptr_t)__builtin_return_address(0));
        return Orig(_this);
    }
};

/*
 * The Skyline version counted lines with std::count(..., "\n") (a char compared with a pointer),
 * so the count was always 0 and this fix never ran. Kept disabled to behave the same;
 * set to true to try it.
 */
inline constexpr bool EnableArtsCraftsDescriptionFix = false;

inline std::string render_temp = "";

HOOK_DEFINE_TRAMPOLINE(RenderText2) {
    static uint64_t Callback(void* _this, int X_Pos, int Y_Pos, const char* string, void* ARGB_Color, void* ARGB_Shadow, void* ARGB_Border, void* x7, float s0, float s1, float fontsize, float X_Scale, double d3, float f1, void* unk8, float* unk9, void* unk10, void* unk11, void* unk12, void* unk13, float* unk14, float* unk15, float* unk16, void* unk17) {
        uintptr_t LR = (uintptr_t)__builtin_return_address(0);
        get_UITextcase(LR);
        if (!UITextcase) {
            ptrdiff_t offsetItr = returnInstructionOffset(LR);
            //Fix Arts and Crafts description
            if (EnableArtsCraftsDescriptionFix && offsetItr == 0x30156C) {
                render_temp = string;
                size_t line_count = std::count(render_temp.begin(), render_temp.end(), '\n');
                if (line_count > 1) {
                    render_temp.erase(0, render_temp.find("\n") + 1);
                    string = render_temp.c_str();
                }
            }
        }
        else {
            switch(UITextcase) {
                //Prompt buttons size and X pos change
                case 6:
                    if (strlen(string) == 0)
                        break;
                    if (strncmp(string, "#", 1) != 0)
                        break;
                    if (strncmp(string+5, ":", 1) == 0)
                        break;
                    if (fontsize == 48.0f)
                        break;
                    fontsize = 32.0;
                    if (X_Pos == 0)
                        break;
                    X_Pos -= 0x30;
                    if (strcmp(string, "#160IHorseRide") == 0)
                        string = "#160IRide Horse";
                    else if (strcmp(string, "#160IBikeRide") == 0)
                        string = "#160IRide Bike";
                    else if (strcmp(string, "#161IHorseDismount") == 0)
                        string = "#161IDismount Horse";
                    else if (strcmp(string, "#161IBikeDismount") == 0)
                        string = "#161IDismount Bike";
                    break;
                //X axis fix for Club affiliations
                case 9:
                    if (X_Pos != 0x483)
                        break;
                    X_Pos = 0x503;
                    break;
                //Fix Travel to printing
                case 12: {
                    std::string text = string;
                    auto index = text.find("Travel to #3C%s#0C?");
                    if (index == std::string::npos)
                        break;
                    render_temp = "Travel to ";
                    render_temp += text.substr(0, index) + std::string("?");
                    string = render_temp.c_str();
                    break;
                }
                //Force item descriptions to use always X_Scale 1.0
                case 13:
                    if (X_Pos != 0x87)
                        break;
                    X_Scale = 1.0;
                    break;
            }
        }

        return Orig(_this, X_Pos, Y_Pos, string, ARGB_Color, ARGB_Shadow, ARGB_Border, x7, s0, s1, fontsize, X_Scale, d3, f1, unk8, unk9, unk10, unk11, unk12, unk13, unk14, unk15, unk16, unk17);
    }
};

HOOK_DEFINE_TRAMPOLINE(RenderTextFromAtlas) {
    static uint64_t Callback(const char* value, int x1, void* x2, float* x3, void* x4, void* x5, float X_Pos, float Y_Pos, float image_X_pos, float image_Y_pos, float elem_width, float elem_height, float kerning) {
        ptrdiff_t offsetItr = returnInstructionOffset((uintptr_t)__builtin_return_address(0));
        static float Last_X = 0;
        static bool cut_decimal = false;
        switch(offsetItr) {
            //Fix fishing timer integer position
            case 0x3534B0: {
                cut_decimal = false;
                if (Y_Pos != 766.0f && X_Pos != 544.0f)
                    break;
                int64_t intgr = strtol(value, NULL, 10);

                if (intgr == 0) {
                    elem_height = 0;
                    cut_decimal = true;
                    break;
                }
                if (intgr < 10) {
                    X_Pos += (44.0 * 2.0);
                    X_Pos -= 30.0;
                    Last_X = (44.0 * 2.0) - 30.0;
                    if (intgr == 1)
                        Last_X -= 16.0;
                }
                else {
                    X_Pos += (44.0 * 3.0);
                    X_Pos -= 20.0;
                    Last_X = (44.0 * 3.0) - 20.0;
                }
                break;
            }
            //Fix fishing timer decimal position
            case 0x353604:
                if (Last_X <= 0.0f)
                    break;

                if (cut_decimal == false)
                    X_Pos += Last_X;
                else cut_decimal = false;
                Last_X = 0.0;
                break;
        }
        return Orig(value, x1, x2, x3, x4, x5, X_Pos, Y_Pos, image_X_pos, image_Y_pos, elem_width, elem_height, kerning);
    }
};

int SetUIText::Callback(void* _this, int X_Pos, int Y_Pos, const char* string, int ARGB_Color, int ARGB_Shadow, int ARGB_Border, int w7, float s0, float s1, float fontsize, float X_Scale) {
    if (strlen(string) == 0)
        return Orig(_this, X_Pos, Y_Pos, string, ARGB_Color, ARGB_Shadow, ARGB_Border, w7, s0, s1, fontsize, X_Scale);

    ptrdiff_t offsetItr = returnInstructionOffset((uintptr_t)__builtin_return_address(0));
    switch(offsetItr) {
        case 0x19774C:
            UITextcase = 5;
            break;
    }

    switch(UITextcase) {
        //Main Menu and Pause Menu
        case 1:
            if ((Y_Pos != 0x24D) && (Y_Pos != 0x295) && (Y_Pos != 0x2D1) && (Y_Pos != 0x325) && (Y_Pos != 0x36D))
                break;

            ReadPads();
            Orig(_this, 1060, 970, "Translation mod made by MasaGratoR & Graber", (int)0xFFFFFFFF, 0x00000000, (int)0xFF000000, w7, s0, s1, fontsize, X_Scale);
            if (AnyPadHeld(nn::hid::KEY_R)) {
                BlockButtons = true;
                RenderHiddenMenu(_this);
            }
            else if (BlockButtons == true) {
                SaveSettings();
                BlockButtons = false;
                indicator = 0;
            }
            break;

        //Save title
        case 2:
            if ((X_Pos != 0x443) || (Y_Pos != 0x120))
                break;
            if (temp.width <= 720)
                break;

            X_Scale = 720.0 / (float)temp.width;
            return Orig(_this, X_Pos, Y_Pos, string, ARGB_Color, ARGB_Shadow, ARGB_Border, w7, s0, s1, fontsize, X_Scale);

        //Enemy Stats
        case 3:
            if ((X_Pos != 0x552) && (X_Pos != 0x60C) && (X_Pos != 0x6C6))
                break;
            if ((Y_Pos != 0x33B) && (Y_Pos != 0x370) && (Y_Pos != 0x3A5) && (Y_Pos != 0x3DA) && (Y_Pos != 0x307))
                break;

            if (temp.width > 60)
                X_Scale = 60.0 / (float)temp.width;
            return Orig(_this, X_Pos, Y_Pos+3, string, ARGB_Color, ARGB_Shadow, ARGB_Border, w7, s0, s1, fontsize*1.2f, X_Scale/1.2f);

        //Enemy stats menu buttons
        case 4:
            if (Y_Pos == 0x40f)
                Y_Pos = 0x413;
            break;
        //Fix for Yes/No, CP/EP
        case 5:
            if ((strcmp("Yes", string) == 0) || (strcmp("No", string) == 0))
                Y_Pos += 7;
            else if ((strncmp("EP ", string, 3) == 0) || (strncmp("CP ", string, 3) == 0)) {
                std::string temp_str = string;
                if (temp_str.length() != 6)
                    break;

                std::string string_pre = temp_str.substr(0, 2);
                std::string string_aft = temp_str.substr(3, 3);
                string_aft.erase(std::remove(string_aft.begin(), string_aft.end(), ' '), string_aft.end());
                Orig(_this, 0x473, Y_Pos, string_pre.c_str(), ARGB_Color, ARGB_Shadow, ARGB_Border, w7, s0, s1, fontsize, X_Scale);
                int new_X = 0x4F0 - CPEP_width;
                if (temp_str.substr(5, 1).compare("5") == 0)
                    new_X -= 3;
                return Orig(_this, new_X, Y_Pos, string_aft.c_str(), ARGB_Color, ARGB_Shadow, ARGB_Border, w7, s0, s1, fontsize, X_Scale);
            }
            break;
        //Fix Y pos for Prompt Buttons
        case 6:
            if (Y_Pos == 0x40c) {
                Y_Pos += 3;
            }
            break;
        //Change rods descriptions size in case of being too long
        case 7:
            if (Y_Pos != 0x2e5 && Y_Pos != 0x2b1)
                break;
            if (X_Pos != 0x21c)
                break;
            if (temp.width > 832)
                X_Scale = 832.0 / (float)temp.width;
            return Orig(_this, X_Pos, Y_Pos, string, ARGB_Color, ARGB_Shadow, ARGB_Border, w7, s0, s1, fontsize, X_Scale);
        //Center Rods string
        case 8:
            if (strcmp(string, "Rods") == 0)
                X_Pos = 0x392;
            break;
        //Craft/Arts names in battle
        case 10:
            if ((X_Pos != 0x394) && (X_Pos != 0x2C1))
                break;
            if (temp.width > 410)
                X_Scale = 410.0 / (float)temp.width;
            break;
        //Blade Help button
        case 11:
            if (X_Pos == 0x6E1) {
                X_Pos -= 17;
                Y_Pos += 2;
            }
            break;
        //Fix item descriptions to not go out of bounds
        case 13:
            if (X_Pos != 0x87)
                break;
            if (temp.width > 1354)
                X_Scale = 1354.0 / (float)temp.width;
            break;
    }

    return Orig(_this, X_Pos, Y_Pos, string, ARGB_Color, ARGB_Shadow, ARGB_Border, w7, s0, s1, fontsize, X_Scale);
}
