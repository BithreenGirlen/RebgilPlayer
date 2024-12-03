#ifndef REBGIL_H_
#define REBGIL_H_

#include <string>
#include <vector>

#include "adv.h"

namespace rebgil
{
	bool InitialiseSetting();
	const std::string& GetFontFilePath();
	const bool IsSkelBinary();

	void GetSpineList(const std::wstring& wstrFolderPath, std::vector<std::string>& atlasPaths, std::vector<std::string>& skelPaths);

	std::wstring DeriveScenarioFilePathFromSpineFolderPath(const std::wstring& wstrFolderPath);
	void LoadScenario(const std::wstring& wstrFilePath, std::vector<adv::TextDatum>& textData);

	bool IsSlotToBeLeftOut(const char* szSlotName, size_t nSlotNameLength);
	std::vector<std::string> GetLeaveOutListIfItWereIrregularScene(const std::string& atlasPath);
}

#endif // !REBGIL_H_
