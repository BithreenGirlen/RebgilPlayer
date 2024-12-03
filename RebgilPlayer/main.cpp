

#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <locale.h>

/*SFML*/
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "winmm.lib")

#ifdef  _DEBUG
#pragma comment(lib, "sfml-system-d.lib")
#pragma comment(lib, "sfml-graphics-d.lib")
#pragma comment(lib, "sfml-window-d.lib")
#else
#pragma comment(lib, "sfml-system.lib")
#pragma comment(lib, "sfml-graphics.lib")
#pragma comment(lib, "sfml-window.lib")
#endif // _DEBUG

#include "win_dialogue.h"
#include "win_filesystem.h"
#include "sfml_main_window.h"
#include "rebgil.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	setlocale(LC_ALL, ".utf8");

	std::wstring wstrPickedFolder = win_dialogue::SelectWorkFolder(nullptr);
	if (wstrPickedFolder.empty())return 0;

	if (!rebgil::InitialiseSetting())return 0;

	CSfmlMainWindow sSfmlMainWindow;
	sSfmlMainWindow.SetFont(rebgil::GetFontFilePath());

	std::vector<std::wstring> folders;
	size_t nFolderIndex = 0;
	win_filesystem::GetFilePathListAndIndex(wstrPickedFolder, nullptr, folders, &nFolderIndex);
	for (;;)
	{
		std::wstring wstrFolderPath = folders.at(nFolderIndex);

		std::vector<std::string> atlasPaths;
		std::vector<std::string> skelPaths;
		rebgil::GetSpineList(wstrFolderPath, atlasPaths, skelPaths);
		if (atlasPaths.empty() || skelPaths.empty())break;

		std::wstring wstrDialogueFilePath = rebgil::DeriveScenarioFilePathFromSpineFolderPath(wstrFolderPath);
		std::vector<adv::TextDatum> textData;
		rebgil::LoadScenario(wstrDialogueFilePath, textData);

		/* Disable rendering multiple spines. */
		atlasPaths.resize(1);
		skelPaths.resize(1);
		bool bRet = sSfmlMainWindow.SetSpineFromFile(atlasPaths, skelPaths, rebgil::IsSkelBinary());
		if (!bRet)break;

		std::vector<std::string> slotsToExclude = rebgil::GetLeaveOutListIfItWereIrregularScene(atlasPaths[0]);
		if (slotsToExclude.empty())
		{
			sSfmlMainWindow.SetSlotExclusionCallback(&rebgil::IsSlotToBeLeftOut);
		}
		else
		{
			sSfmlMainWindow.SetSlotsToExclude(slotsToExclude);
		}

		sSfmlMainWindow.SetTexts(textData);

		int iRet = sSfmlMainWindow.Display();
		if (iRet == 1)
		{
			++nFolderIndex;
			if (nFolderIndex >= folders.size())nFolderIndex = 0;
		}
		else if (iRet == 2)
		{
			--nFolderIndex;
			if (nFolderIndex >= folders.size())nFolderIndex = folders.size() - 1;
		}
		else
		{
			break;
		}
	}
	return 0;
}