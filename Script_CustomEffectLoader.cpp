#include "Script_CustomEffectLoader.h"

#include "util\Memory.h"
#include "util\Logging.h"
#include "util\Hook.h"
#include "util/Util.h"

#include "Script.h"
#include "iostream"


using std::cout;
gSScriptInit & GetScriptInit()
{
	static gSScriptInit s_ScriptInit;
	return s_ScriptInit;
}


void SearchAndMergeEffects(const bCString& directory) {
	bTObjArray<bCString> arrFiles;
	bTObjArray<bCString> arrDirs;
	eCVirtualFileSystem& vFileSystem = eCVirtualFileSystem::GetInstance();

	auto EffectModule = eCModuleAdmin::GetInstance().FindModule("gCEffectModule");
	auto EffectModulePtr = reinterpret_cast<DWORD>(&EffectModule);

	if (EffectModulePtr == 0)
		return;

	DWORD gCEffectSystemPtr = *(DWORD*)EffectModulePtr + 0x14;
	if (gCEffectSystemPtr == 0)
		return;

	gCEffectMap* EffectMap = (gCEffectMap*)(*(DWORD*)gCEffectSystemPtr + 0x4);
	if (EffectMap == nullptr)
		return;

	vFileSystem.FindFiles(directory, arrFiles);
	vFileSystem.FindDirectories(directory, arrDirs);

	for (auto it = arrFiles.Begin(); it != arrFiles.End(); it++) {
		bTObjArray< bCString > fileSplitted = SplitString(*it, ".", GEFalse, GEFalse);
		bCString fileName = fileSplitted.GetAt(0);
		bCString fileExt = fileSplitted.GetAt(1);

		// Ignore original g3.efm
		if (fileExt != "efm" || fileName == "g3") continue;

		bTObjArray< bCString > dirSplitted = SplitString(directory, "/", GEFalse, GEFalse);
		dirSplitted.Add(*it);
		bCString filePath = JoinString(dirSplitted, 0, "/");

		

		gCEffectMap NewEM;
		NewEM.Load(filePath);
		for (auto iter = NewEM.Begin(); iter != NewEM.End(); iter++) {
			cout << "EffectName: " << iter.GetKey() << "\n";
			EffectMap->RemoveAt(iter.GetKey());
			gCEffectCommandSequence* effectCommand = EffectMap->InsertNewAt(iter.GetKey());
			*effectCommand = iter.GetNode()->m_Element;
		}
		cout << filePath << " merged!" << "\n";
	}

	// Recursive search
	for (auto it = arrDirs.Begin(); it != arrDirs.End(); it++) {
		bCString subDirPath = directory + "/" + *it;
		SearchAndMergeEffects(subDirPath);
	}
}

static mCCallHook Hook_AfterApplicationProcess;
void GE_STDCALL AfterApplicationProcess(void)
{
	Hook_AfterApplicationProcess.Disable();
	SearchAndMergeEffects("Data");
}



bTPropertyObject<mCCustomEffectLoader, eCEngineComponentBase> mCCustomEffectLoader::ms_PropertyObjectInstance_mCCustomEffectLoader(GETrue);

void mCCustomEffectLoader::Process(void) {

}

mCCustomEffectLoader::~mCCustomEffectLoader(void) {
	
}

mCCustomEffectLoader::mCCustomEffectLoader(void) {
	eCModuleAdmin::GetInstance().RegisterModule(*this);
}

extern "C" __declspec(dllexport)
gSScriptInit const * GE_STDCALL ScriptInit(void)
{
	GetScriptAdmin().LoadScriptDLL("Script_Game.dll");

	Hook_AfterApplicationProcess
		.Prepare(RVA_Engine(0x1677C), &AfterApplicationProcess)
		.InsertCall().Hook();


	static bCAccessorCreator Test(bTClassName<mCCustomEffectLoader>::GetUnmangled());

	return &GetScriptInit();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		::DisableThreadLibraryCalls(hModule);
		//AllocConsole();
		//freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
