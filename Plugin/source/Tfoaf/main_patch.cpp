#include "main_patch.hpp"
#include "Hiddenmenu.hpp"
#include "TextRendering.hpp"

int (*fs_openfile_original)(nn::fs::FileHandle* handle, const char* filepath, int mode);
int fs_openfile_hook(nn::fs::FileHandle* handle, const char* filepath, int mode) {
	static bool initialized = false;
	if (!initialized) {
		s64 scripts_count = 0;
		s64 voice_count = 0;
		nn::fs::DirectoryHandle dirhandle;
		if(!nn::fs::OpenDirectory(&dirhandle, "rom:/data/scripts_EN", nn::fs::OpenDirectoryMode_All)) {
			nn::fs::GetDirectoryEntryCount(&scripts_count, dirhandle);
			nn::fs::CloseDirectory(dirhandle);
		}
		if(!nn::fs::OpenDirectory(&dirhandle, "rom:/sound/stream/EN", nn::fs::OpenDirectoryMode_All)) {
			nn::fs::GetDirectoryEntryCount(&voice_count, dirhandle);
			nn::fs::CloseDirectory(dirhandle);
		}
		if (!scripts_count) EngCheck = 1;
		if (!voice_count) EngCheck += 2;
		nn::fs::MountSdCardForDebug("sd");
		nn::fs::FileHandle handle;
		s64 filesize = 0;
		if (!fs_openfile_original(&handle, "sd:/config/ToCS2/subsdk9.save", nn::fs::OpenMode_Read)) {
			nn::fs::GetFileSize(&filesize, handle);
				char magic_check[5] = "";
				nn::fs::ReadFile(handle, 0, &magic_check, 4);
				if (strcmp(magic, magic_check) == 0)
					nn::fs::ReadFile(handle, 4, &Settings, filesize - 4);
				nn::fs::CloseFile(handle);
		}
		else {
			nn::fs::CreateDirectory("sd:/config");
			nn::fs::CreateDirectory("sd:/config/ToCS2");
			nn::fs::CreateFile("sd:/config/ToCS2/subsdk9.save", 4+sizeof(Settings));
		}
		if (Settings.GPUBoost == 0) nn::oe::SetPerformanceConfiguration(nn::oe::Normal, PerformanceConfig_GPU307mhz);
		else nn::oe::SetPerformanceConfiguration(nn::oe::Normal, PerformanceConfig_GPU460mhz);
		initialized = true;
	}
	if (enSelected) {
		if (!strncmp("rom:/data/scripts", filepath, sizeof("rom:/data/scripts"))) {
			nn::fs::FileHandle filehandle;
			std::string new_string = "rom:/data/scripts_EN";
			std::string temp = filepath;
			temp = temp.substr(sizeof("rom:/data/scripts"));
			new_string += temp;
			if(!fs_openfile_original(&filehandle, new_string.c_str(), nn::fs::OpenMode_Read)) {
				nn::fs::CloseFile(filehandle);
				filepath = new_string.c_str();
			}
		}
		else if (!strncmp("rom:/sound/stream", filepath, sizeof("rom:/sound/stream"))) {
			nn::fs::FileHandle filehandle;
			std::string new_string = "rom:/sound/stream/EN";
			std::string temp = filepath;
			temp = temp.substr(sizeof("rom:/sound/stream"));
			new_string += temp;
			if(!fs_openfile_original(&filehandle, new_string.c_str(), nn::fs::OpenMode_Read)) {
				nn::fs::CloseFile(filehandle);
				filepath = new_string.c_str();
			}
		}
	}
	return fs_openfile_original(handle, filepath, mode);
}

uint64_t (*GetNpadState_Pro_original)(nn::hid::NpadFullKeyState * KeyState, unsigned int const& NpadID);
uint64_t GetNpadState_Pro_hook(nn::hid::NpadFullKeyState * KeyState, unsigned int const& NpadID) {
	uint64_t ret = GetNpadState_Pro_original(KeyState, NpadID);
	if (BlockButtons)
		if (KeyState != &out3) {
			KeyState->Buttons = nn::hid::KEY_R;
			KeyState->LStickY = 0;
		}
	return ret;
}

uint64_t (*GetNpadState_JoyCons_original)(nn::hid::NpadJoyDualState * KeyState, unsigned int const& NpadID);
uint64_t GetNpadState_JoyCons_hook(nn::hid::NpadJoyDualState * KeyState, unsigned int const& NpadID) {
	uint64_t ret = GetNpadState_JoyCons_original(KeyState, NpadID);
	if (BlockButtons)
		if (KeyState != &out2) {
			KeyState->Buttons = nn::hid::KEY_R;
			KeyState->LStickY = 0;
		}
	return ret;
}

uint64_t (*GetNpadState_Handheld_original)(nn::hid::NpadHandheldState * KeyState, unsigned int const& NpadID);
uint64_t GetNpadState_Handheld_hook(nn::hid::NpadHandheldState * KeyState, unsigned int const& NpadID) {
	uint64_t ret = GetNpadState_Handheld_original(KeyState, NpadID);
	if (BlockButtons)
		if (KeyState != &out) {
			KeyState->Buttons = nn::hid::KEY_R;
			KeyState->LStickY = 0;
		}
	return ret;
}

const char* cmd[3] = {(char*)&"ToCS1", (char*)&"-msaa=8"};


int (*GetArgc_original)();
int GetArgc_hook() {
	return 2;
}

const char** (*GetArgv_original)();
const char** GetArgv_hook() {
	return (const char**)&cmd;
}

void CS2_main()
{

	NSO_main_start = (uintptr_t)skyline::utils::getRegionAddress(skyline::utils::region::Text);

	//Hook Function calling UI Text Render
	A64HookFunction((void*)(NSO_main_start+0x47A400),
			reinterpret_cast<void*>(RenderText_hook),
			(void**)&RenderText_original);
	 A64HookFunction((void*)(NSO_main_start+0x3015A0),
	 		reinterpret_cast<void*>(RenderText2_hook),
	 		(void**)&RenderText2_original);
	A64HookFunction((void*)(NSO_main_start+0x1968D0),
			reinterpret_cast<void*>(RenderText3_hook),
			(void**)&RenderText3_original);
	A64HookFunction((void*)(NSO_main_start+0x3519D0),
	 		reinterpret_cast<void*>(RenderText4_hook),
	 		(void**)&RenderText4_original);
	A64HookFunction((void*)(NSO_main_start+0x4867E0),
	 		reinterpret_cast<void*>(RenderTextFromAtlas_hook),
	 		(void**)&RenderTextFromAtlas_original);

	//Hook UI Text Render
	A64HookFunction((void*)(NSO_main_start+0x300A90),
			reinterpret_cast<void*>(set_UIText_hook),
			(void**)&set_UIText_original);

	//Hook Text Width Calcs
	A64HookFunction((void*)(NSO_main_start+0x260230),
			reinterpret_cast<void*>(get_TextWidth_hook),
			(void**)&get_TextWidth_original);
	A64HookFunction((void*)(NSO_main_start+0x260EB0),
			reinterpret_cast<void*>(get_TextWidth2_hook),
			(void**)&get_TextWidth2_original);
	
	//Hook Text Kerning
	A64HookFunction((void*)(NSO_main_start+0x25FD00),
			reinterpret_cast<void*>(set_TextKerning_hook),
			(void**)&set_TextKerning_original);
	
	//Hook vsnprintf wrapper
	A64HookFunction((void*)(NSO_main_start+0x5CA00),
			reinterpret_cast<void*>(vsnprintfWrapper_hook),
			(void**)&vsnprintfWrapper_original);
	
	//Hook strncat
	A64HookFunction((void*)(NSO_main_start+0x83D6F0),
			reinterpret_cast<void*>(strncat_hook),
			(void**)&strncat_original);
	
	//Hook FS
	A64HookFunction((void*)&nn::fs::OpenFile,
			reinterpret_cast<void*>(fs_openfile_hook),
			(void**)&fs_openfile_original);
	
	//Hook FPS lock
	A64HookFunction((void*)(NSO_main_start+0x5D100),
			reinterpret_cast<void*>(FPSlock_hook),
			(void**)&FPSlock_original);
	
	//Hook buttons
	A64HookFunction((void**)&_ZN2nn3hid12GetNpadStateEPNS0_16NpadFullKeyStateERKj,
		reinterpret_cast<void*>(GetNpadState_Pro_hook),
		(void**)&GetNpadState_Pro_original);
	A64HookFunction((void**)&_ZN2nn3hid12GetNpadStateEPNS0_16NpadJoyDualStateERKj,
		reinterpret_cast<void*>(GetNpadState_JoyCons_hook),
		(void**)&GetNpadState_JoyCons_original);
	A64HookFunction((void**)&_ZN2nn3hid12GetNpadStateEPNS0_17NpadHandheldStateERKj,
		reinterpret_cast<void*>(GetNpadState_Handheld_hook),
		(void**)&GetNpadState_Handheld_original);
	
	//Hook argv
	// A64HookFunction((void*)&nn::os::GetHostArgc,
	// 		reinterpret_cast<void*>(GetArgc_hook),
	// 		(void**)&GetArgc_original);
	// A64HookFunction((void*)&nn::os::GetHostArgv,
	// 		reinterpret_cast<void*>(GetArgv_hook),
	// 		(void**)&GetArgv_original);
	
	//Hook 3D world rendering res
	 A64HookFunction((void*)(NSO_main_start+0x2E5260),
	 		reinterpret_cast<void*>(RenderingRes_hook),
	 		(void**)&RenderingRes_original);

	uint8_t patch1[4] = {0x08, 0x0E, 0x00, 0xB5}; //cbnz x8, #0x1c0
	uint8_t NOP[4] = {0x1f, 0x20, 0x03, 0xD5}; //NOP
	uint8_t s0float16[4] = {0x00, 0x10, 0x26, 0x1E}; // fmov s0,#16.0
	uint8_t movx0_0[4] = {0x00, 0x00, 0x80, 0xD2}; //mov x0, #0
	//Patch save description to not add character names + their levels.
	//Formatting included in game is breaking under longer names like "Instructor Sara"
	//which locks game when trying to format it
	//sky_memcpy((void*)(NSO_main_start+0x150928), &patch1, 4);

	//Force game to use nvnSamplerBuilderSetMaxAnisotropy
	sky_memcpy((void*)(NSO_main_start+0x78EEDC), &NOP, 4);
	//Force nvnSamplerBuilderSetMaxAnisotropy to set anisotropy to 16.0
	sky_memcpy((void*)(NSO_main_start+0x78EEE4), &s0float16, 4);

}