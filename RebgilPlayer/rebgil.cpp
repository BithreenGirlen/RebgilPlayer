
#include <memory>
#include <unordered_map>

#include "rebgil.h"

#include "json_minimal.h"
#include "text_utility.h"
#include "win_filesystem.h"
#include "win_text.h"

namespace rebgil
{
	struct SPlayerSetting
	{
		std::wstring wstrAtlasExtension;
		std::wstring wstrSkelExtension;
		std::wstring wstrVoiceExtension;
		std::wstring wstrSceneTextExtension;
		std::string strFontFilePath;

		bool bSkelBinary = true;
		bool bToTranslateText = true;
	};

	struct SDialogueParams
	{
		std::string strTextId;

		std::wstring wstrText;
		std::wstring wstrVoiceFileName;
	};

	static SPlayerSetting g_playerSetting;


	static void SetupDefaultSetting()
	{
		g_playerSetting.wstrAtlasExtension = L"atlas.txt";
		g_playerSetting.wstrSkelExtension = L"skel.txt";
		g_playerSetting.wstrVoiceExtension = L".m4a";
		g_playerSetting.wstrSceneTextExtension = L".txt";
		g_playerSetting.strFontFilePath = "C:\\Windows\\Fonts\\yumindb.ttf";

		g_playerSetting.bSkelBinary = true;
		g_playerSetting.bToTranslateText = true;
	}

	static bool ReadSettingFile(SPlayerSetting &playerSetting)
	{
		std::wstring wstrFilePath = win_filesystem::GetCurrentProcessPath() + L"\\setting.txt";
		std::string strFile = win_filesystem::LoadFileAsString(wstrFilePath.c_str());
		if (strFile.empty())return false;

		char* p = &strFile[0];
		auto pp = std::make_unique<char*>();

		json_minimal::ExtractJsonObject(&p, "extension", &*pp);
		if (*pp == nullptr)return false;

		std::vector<char> vBuffer(1024, '\0');
		bool bRet = json_minimal::GetJsonElementValue(*pp, "atlas", vBuffer.data(), vBuffer.size());
		if (!bRet)return false;
		playerSetting.wstrAtlasExtension = win_text::WidenUtf8(vBuffer.data());

		bRet = json_minimal::GetJsonElementValue(*pp, "skel", vBuffer.data(), vBuffer.size());
		if (!bRet)return false;
		playerSetting.wstrSkelExtension = win_text::WidenUtf8(vBuffer.data());

		bRet = json_minimal::GetJsonElementValue(*pp, "voice", vBuffer.data(), vBuffer.size());
		if (!bRet)return false;
		playerSetting.wstrVoiceExtension = win_text::WidenUtf8(vBuffer.data());

		bRet = json_minimal::GetJsonElementValue(*pp, "sceneText", vBuffer.data(), vBuffer.size());
		if (!bRet)return false;
		playerSetting.wstrSceneTextExtension = win_text::WidenUtf8(vBuffer.data());

		bRet = json_minimal::GetJsonElementValue(p, "binarySkel", vBuffer.data(), vBuffer.size());
		if (!bRet)return false;
		playerSetting.bSkelBinary = strcmp(vBuffer.data(), "true") == 0;

		bRet = json_minimal::GetJsonElementValue(p, "fontPath", vBuffer.data(), vBuffer.size());
		if (!bRet)return false;
		playerSetting.strFontFilePath = vBuffer.data();

		bRet = json_minimal::GetJsonElementValue(p, "translation", vBuffer.data(), vBuffer.size());
		if (!bRet)return false;
		playerSetting.bToTranslateText = strcmp(vBuffer.data(), "true") == 0;

		return true;
	}
	/*脚本ファイル経路 => 音声フォルダ階層*/
	static std::wstring DeriveVoiceFolderPathFromScenarioFilePath(const std::wstring& wstrFilePath)
	{
		size_t nPos = wstrFilePath.find(L"dialogxml");
		if (nPos == std::wstring::npos)return std::wstring();

		return wstrFilePath.substr(0, nPos) + L"dialogvoice\\";
	}

	static void UnescapeXML(std::wstring& wstrText)
	{
		const wchar_t swzLineBreak[] = L"&#xA;";
		const wchar_t swzLessThan[] = L"&lt;";
		const wchar_t swzGreaterThan[] = L"&gt;";

		text_utility::ReplaceAll(wstrText, swzLineBreak, L"");

		for (size_t nRead = 0;;)
		{
			size_t nStart = wstrText.find(swzLessThan, nRead);
			if (nStart == std::wstring::npos)return;

			size_t nEnd = wstrText.find(swzGreaterThan, nStart);
			if (nEnd == std::wstring::npos)return;

			nEnd += sizeof(swzGreaterThan) / sizeof(wchar_t) - 1;
			std::wstring wstr = wstrText.substr(nStart, nEnd - nStart);
			text_utility::ReplaceAll(wstrText, wstr, L"");

			nRead = nStart;
		}
	};

	static long long  FindAttribute(const std::vector<std::pair<std::string, std::string>>& attributes, const std::string& str)
	{
		for (size_t i = 0; i < attributes.size(); ++i)
		{
			if (attributes[i].first == str)
			{
				return i;
			}
		}
		return -1;
	}

	static void LoadScenarioWithoutTranslating(const std::vector<std::string> plotTags, std::vector<adv::TextDatum>& textData, const std::wstring &wstrVoiceFolderPath)
	{
		std::vector<std::pair<std::string, std::string>> attributes;
		textData.reserve(plotTags.size());
		for (const auto& plotTag : plotTags)
		{
			adv::TextDatum t;

			text_utility::GetXmlAttributes(plotTag, attributes);

			long long llIndex = FindAttribute(attributes, "show_type");
			if (llIndex == -1 || attributes[llIndex].second != "InsertDialog")
			{
				continue;
			}

			llIndex = FindAttribute(attributes, "parameter5");
			if (llIndex == -1)continue;

			t.wstrText = win_text::WidenUtf8(attributes[llIndex].second);
			UnescapeXML(t.wstrText);

			llIndex = FindAttribute(attributes, "parameter6");
			if (llIndex != -1)
			{
				t.wstrVoicePath = wstrVoiceFolderPath + win_text::WidenUtf8(attributes[llIndex].second) + g_playerSetting.wstrVoiceExtension;
			}
			textData.push_back(std::move(t));
		}
	}
}

bool rebgil::InitialiseSetting()
{
	SPlayerSetting playerSetting;
	bool bRet = ReadSettingFile(playerSetting);
	if (bRet)
	{
		g_playerSetting = std::move(playerSetting);
	}
	else
	{
		SetupDefaultSetting();
	}

	return g_playerSetting.wstrAtlasExtension != g_playerSetting.wstrSkelExtension;
}

const std::string& rebgil::GetFontFilePath()
{
	return g_playerSetting.strFontFilePath;
}

const bool rebgil::IsSkelBinary()
{
	return g_playerSetting.bSkelBinary;
}
/*Spine素材一覧取得*/
void rebgil::GetSpineList(const std::wstring& wstrFolderPath, std::vector<std::string>& atlasPaths, std::vector<std::string>& skelPaths)
{
	bool bAtlasLonger = g_playerSetting.wstrAtlasExtension.size() > g_playerSetting.wstrSkelExtension.size();

	std::wstring& wstrLongerExtesion = bAtlasLonger	? g_playerSetting.wstrAtlasExtension : g_playerSetting.wstrSkelExtension;
	std::wstring& wstrShorterExtension = bAtlasLonger ? g_playerSetting.wstrSkelExtension : g_playerSetting.wstrAtlasExtension;
	std::vector<std::string>& strLongerPaths = bAtlasLonger	? atlasPaths : skelPaths;
	std::vector<std::string>& strShorterPaths = bAtlasLonger ? skelPaths : atlasPaths;

	std::vector<std::wstring> wstrFilePaths;
	win_filesystem::CreateFilePathList(wstrFolderPath.c_str(), L"*", wstrFilePaths);

	for (const auto& filePath : wstrFilePaths)
	{
		const auto EndsWith = [filePath](const std::wstring& str)
			-> bool
			{
				if (filePath.size() < str.size()) return false;
				return std::equal(str.rbegin(), str.rend(), filePath.rbegin());
			};

		if (EndsWith(wstrLongerExtesion))
		{
			strLongerPaths.push_back(win_text::NarrowUtf8(filePath));
		}
		else if (EndsWith(wstrShorterExtension))
		{
			strShorterPaths.push_back(win_text::NarrowUtf8(filePath));
		}

	}

}
/*Spine素材フォルダ経路 => 台本ファイル経路*/
std::wstring rebgil::DeriveScenarioFilePathFromSpineFolderPath(const std::wstring& wstrFolderPath)
{
	const auto ExtractScenarioId = [&wstrFolderPath]()
		-> std::wstring
		{
			size_t nPos1 = wstrFolderPath.find_last_of(L"\\/");
			if (nPos1 == std::wstring::npos)return std::wstring();
			size_t nPos2 = wstrFolderPath.find_last_of(L"\\/", nPos1);
			if (nPos2 == std::wstring::npos)return std::wstring();
			++nPos2;

			return wstrFolderPath.substr(nPos2, nPos1 - nPos2);
		};
	std::wstring wstrScenarioId = ExtractScenarioId();
	if (wstrScenarioId.size() < 2)return std::wstring();
	wstrScenarioId[wstrScenarioId.size() - 2] = L'6';

	size_t nPos = wstrFolderPath.rfind(L"bundles");
	if (nPos == std::wstring::npos)return std::wstring();

	std::wstring wstrScenarioPath = wstrFolderPath.substr(0, nPos) 
		+ L"dialogxml\\jp\\" + wstrScenarioId 
		+ L"\\dialog_show" + g_playerSetting.wstrSceneTextExtension;
	return wstrScenarioPath;
}
/*台本ファイル読み取り*/
void rebgil::LoadScenario(const std::wstring& wstrFilePath, std::vector<adv::TextDatum>& textData)
{
	std::string strFile = win_filesystem::LoadFileAsString(wstrFilePath.c_str());
	if (strFile.empty())return;

	std::wstring wstrVoiceFolderPath = DeriveVoiceFolderPathFromScenarioFilePath(wstrFilePath);
	if (wstrVoiceFolderPath.empty())return;

	/*
	* 1. "dialog_show.txt"から文章IDと音声ファイル経路を抜粋
	* 2. "dialog_text.txt"から翻訳文章と対応ID取得
	* 3. 翻訳文章と音声ファイルを結び付け
	*/

	std::vector<std::string> plotTags;
	text_utility::ToXmlTags(strFile, "dialog_show", plotTags);

	if (!g_playerSetting.bToTranslateText)
	{
		LoadScenarioWithoutTranslating(plotTags, textData, wstrVoiceFolderPath);
		return;
	}

	const auto ExtractDirectory = [](const std::wstring& wstrFilePath)
		-> std::wstring
		{
			size_t nPos = wstrFilePath.find_last_of(L"\\/");
			if (nPos != std::wstring::npos)
			{
				return wstrFilePath.substr(0, nPos);
			}
			return wstrFilePath;
		};

	std::wstring wstrPlotFilePath = ExtractDirectory(wstrFilePath) + L"\\dialog_text" + g_playerSetting.wstrSceneTextExtension;
	strFile = win_filesystem::LoadFileAsString(wstrPlotFilePath.c_str());
	if (strFile.empty())return;

	std::vector<std::string> textTags;
	text_utility::ToXmlTags(strFile, "dialog_text", textTags);

	std::vector<std::pair<std::string, std::string>> attributes;

	std::unordered_map<std::string, std::wstring> translatedTexts;
	for (const auto& textTag : textTags)
	{
		text_utility::GetXmlAttributes(textTag, attributes);

		long long llIndex = FindAttribute(attributes, "text");
		if (llIndex == -1)continue;

		std::wstring wstrText = win_text::WidenUtf8(attributes[llIndex].second);
		UnescapeXML(wstrText);

		llIndex = FindAttribute(attributes, "id");
		if (llIndex == -1)continue;

		translatedTexts.insert({ attributes[llIndex].second, wstrText });
	}

	textData.reserve(translatedTexts.size());
	for (const auto& plotTag : plotTags)
	{
		text_utility::GetXmlAttributes(plotTag, attributes);

		long long llIndex = FindAttribute(attributes, "parameter4");
		if (llIndex == -1)continue;

		const auto& iter = translatedTexts.find(attributes[llIndex].second);
		if (iter == translatedTexts.cend())continue;

		llIndex = FindAttribute(attributes, "parameter6");
		
		adv::TextDatum t;
		t.wstrText = iter->second;
		if (llIndex != -1)
		{
			t.wstrVoicePath = wstrVoiceFolderPath + win_text::WidenUtf8(attributes[llIndex].second) + g_playerSetting.wstrVoiceExtension;
		}
		textData.push_back(std::move(t));
	}
}

bool rebgil::IsSlotToBeLeftOut(const char* szSlotName, size_t nSlotNameLength)
{
	const auto IsMaskSlotCandidate = [&szSlotName, &nSlotNameLength]()
		-> bool
		{
			return nSlotNameLength > 2 && szSlotName[nSlotNameLength - 1] == 'M' && szSlotName[nSlotNameLength - 2] == '_';
		};
	const auto IsSensitiveSlot = [&szSlotName, &nSlotNameLength]()
		-> bool
		{
			const char* szSensitiveSlots[] =
			{
				"Anus", "Dick", "dick", "Egg", "egg", "Pussy", "pussy", "Toy", "JJ", "mako", "water"
			};

			for (size_t i = 0; i < sizeof(szSensitiveSlots) / sizeof(szSensitiveSlots[0]); ++i)
			{
				if (strstr(szSlotName, szSensitiveSlots[i]) != nullptr)return true;
			}

			return false;
		};
	const auto IsGeneralMask = [&szSlotName, &nSlotNameLength]()
		-> bool
		{
			return strstr(szSlotName, "M/MM") != nullptr || strstr(szSlotName, "mmm") != nullptr;
		};

	return (IsMaskSlotCandidate() && IsSensitiveSlot()) || IsGeneralMask();
}

std::vector<std::string> rebgil::GetLeaveOutListIfItWereIrregularScene(const std::string& atlasPath)
{
	const std::unordered_map<std::string, std::vector<std::string>> irrergularMaskSlotMap
	{
		{"2014_CG2", { "B_Pussy_U3", "B_Pussy_UB3", "B_Pussy_UU2", "Man_Dick4", "Man_Dick5"}},
		{"2018_CG1", {"HI2", "Hip2"}},
		{"2022", {"Man_Dick_IN2", "Man_Dick_Out2", "Man_Dick_Shadow2", "Pussy_Close2", "Pussy_B2", "Pussy_F2"}},
		{"2032_CG2", {"B_Pussy_1b2", "B_Pussy_2b", "B_Pussy_3b","MAN_dick2"}},
		{"2042_CG2", {"B_pussy_3","MAN_Egg2", "MAN_dick2"}},
		{"2049_CG2", {"Man_Dick_act1_tr2", "Man_Dick_act2-6", "Pussy_B2", "Pussy_Close_act2", "Pussy_Shadow_act2-6", "Pussy_F2"}}
	};

	const auto ExtractFileName = [&atlasPath]()
		-> std::string
		{
			std::vector<std::string> splits;
			text_utility::SplitTextBySeparator(atlasPath, '\\', splits);
			if (splits.empty())return atlasPath;

			size_t nPos = splits[splits.size() - 1].find('.');
			if (nPos == std::string::npos)return splits[splits.size() - 1];

			return splits[splits.size() - 1].substr(0, nPos);
		};

	const auto fileName = ExtractFileName();

	const auto iter = irrergularMaskSlotMap.find(fileName);
	if (iter != irrergularMaskSlotMap.cend())
	{
		return iter->second;
	}
	return std::vector<std::string>();
}
