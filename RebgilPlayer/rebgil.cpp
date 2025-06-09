
#include <memory>
#include <unordered_map>

#include "rebgil.h"

#include "json_minimal.h"
#include "text_utility.h"
#include "path_utility.h"
#include "win_filesystem.h"
#include "win_text.h"

namespace rebgil
{
	struct SPlayerSetting
	{
		std::wstring wstrAtlasExtension = L".atlas";
		std::wstring wstrSkelExtension = L".skel";
		std::wstring wstrVoiceExtension = L".m4a";
		std::wstring wstrSceneTextExtension = L".xml";
		std::string strFontFilePath = "C:\\Windows\\Fonts\\yumindb.ttf";

		bool bToTranslateText = true;
	};

	static SPlayerSetting g_playerSetting;

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

		text_utility::ReplaceAll(wstrText, swzLineBreak, L"\n");

		for (size_t nRead = 0;;)
		{
			size_t nStart = wstrText.find(swzLessThan, nRead);
			if (nStart == std::wstring::npos)return;

			size_t nEnd = wstrText.find(swzGreaterThan, nStart);
			if (nEnd == std::wstring::npos)return;

			nEnd += sizeof(swzGreaterThan) / sizeof(wchar_t) - 1;
			wstrText.erase(nStart, nEnd - nStart);

			nRead = nStart;
		}
	};

	static long long FindAttribute(const std::vector<std::pair<std::string, std::string>>& attributes, const std::string& str)
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

	struct CommandDatum
	{
		std::string commandType;
		std::unordered_map<std::string, std::string> parameters;
	};

	static void ReadDialogueShowXml(const std::wstring &wstrDialogueFilePath, std::vector<CommandDatum> &commandData)
	{
		std::string strFile = win_filesystem::LoadFileAsString(wstrDialogueFilePath.c_str());
		if (strFile.empty())return;

		std::vector<std::string> tags;
		text_utility::ToXmlTags(strFile, "dialog_show", tags);
		for (const auto& tag : tags)
		{
			std::vector<std::pair<std::string, std::string>> attributes;
			text_utility::GetXmlAttributes(tag, attributes);

			long long llIndex = FindAttribute(attributes, "show_type");
			if (llIndex == -1)continue;

			CommandDatum commandDatum;
			commandDatum.commandType = attributes[llIndex].second;

			for (const auto& attiribute : attributes)
			{
				if (attiribute != attributes[llIndex])
				{
					commandDatum.parameters.insert({ attiribute.first, attiribute.second });
				}
			}

			commandData.push_back(std::move(commandDatum));
		}
	}

	static void ReadDialogueTextXml(const std::wstring& wstrDialogueFilePath, std::unordered_map<std::string, std::string>& textMap)
	{
		std::string strFile = win_filesystem::LoadFileAsString(wstrDialogueFilePath.c_str());
		if (strFile.empty())return;

		std::vector<std::string> tags;
		text_utility::ToXmlTags(strFile, "dialog_text", tags);
		for (const auto& tag : tags)
		{
			std::vector<std::pair<std::string, std::string>> attributes;
			text_utility::GetXmlAttributes(tag, attributes);

			long long llIndex = FindAttribute(attributes, "id");
			if (llIndex == -1)continue;
			const std::string& id = attributes[llIndex].second;

			llIndex = FindAttribute(attributes, "text");
			if (llIndex == -1)continue;
			const std::string &text = attributes[llIndex].second;

			textMap.insert({ id, text });
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

	return g_playerSetting.wstrAtlasExtension != g_playerSetting.wstrSkelExtension;
}

const std::string& rebgil::GetFontFilePath()
{
	return g_playerSetting.strFontFilePath;
}

const bool rebgil::IsSkelBinary()
{
	const wchar_t* wszBinaryCandidates[] =
	{
		L".skel", L".bin"
	};
	for (size_t i = 0; i < sizeof(wszBinaryCandidates) / sizeof(wszBinaryCandidates[0]); ++i)
	{
		if (g_playerSetting.wstrSkelExtension.find(wszBinaryCandidates[i]) != std::wstring::npos)
		{
			return true;
		}
	}
	return false;
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

	std::wstring wstrScenarioPath = wstrFolderPath.substr(0, nPos) + L"dialogxml\\jp\\" + wstrScenarioId + L"\\dialog_show" + g_playerSetting.wstrSceneTextExtension;
	return wstrScenarioPath;
}
/*台本ファイル読み取り*/
void rebgil::LoadScenario(const std::wstring& wstrFilePath, std::vector<adv::TextDatum>& textData, std::vector<std::string>& animationNames)
{
	std::vector<CommandDatum> commandData;
	ReadDialogueShowXml(wstrFilePath, commandData);
	if (commandData.empty())return;

	std::wstring wstrDialogueTextFilePath = path_utility::ExtractDirectory(wstrFilePath) + L"\\dialog_text" + g_playerSetting.wstrSceneTextExtension;
	std::unordered_map<std::string, std::string> textMap;
	if (g_playerSetting.bToTranslateText)
	{
		/* 翻訳文章と対応ID取得 */
		ReadDialogueTextXml(wstrDialogueTextFilePath, textMap);
		if (textMap.empty())return;
	}

	std::wstring wstrVoiceFolderPath = DeriveVoiceFolderPathFromScenarioFilePath(wstrFilePath);
	if (wstrVoiceFolderPath.empty())return;

	size_t nCurrentAnimationIndex = 0;
	for (const auto& commandDatum : commandData)
	{
		const auto& type = commandDatum.commandType;
		const auto& params = commandDatum.parameters;

		/*
		* ---------- The command-types in "dialog_show.xml" ----------
		* - InsertDialog
		*   - parameter1 : character name ID?
		*   - parameter4 : text ID to be referred in "dialog_text.xml"
		*   - parameter5 : text
		*   - parameter6 : voice file name
		*
		* - InsertDynamicCG
		*   - parameter1 : Spine folder name?
		*   - parameter2 : Initial animation name
		*
		* - DynamicCGChangeAction
		*   - parameter1 : Animation name to be set
		*
		* - InsertBackgroundVoice
		*   - parameter1 : background voice file name
		*   - parameter2 : BGV ID
		*
		* - InsertSound
		*   - parameter1 : sound effect file name
		*/
		if (type == "InsertDialog")
		{
			adv::TextDatum t;
			if (g_playerSetting.bToTranslateText)
			{
				const auto& iterId = params.find("parameter4");
				if (iterId == params.cend())continue;

				const auto& iterText = textMap.find(iterId->second);
				if (iterText == textMap.cend())continue;

				t.wstrText = win_text::WidenUtf8(iterText->second);
			}
			else
			{
				const auto& iterRawText = commandDatum.parameters.find("parameter5");
				if (iterRawText == params.cend())continue;

				t.wstrText = win_text::WidenUtf8(iterRawText->second);
			}

			const auto& iterVoice = params.find("parameter6");
			if (iterVoice != params.cend())
			{
				t.wstrVoicePath = wstrVoiceFolderPath + win_text::WidenUtf8(iterVoice->second) + g_playerSetting.wstrVoiceExtension;
			}

			UnescapeXML(t.wstrText);
			t.nAnimationIndex = nCurrentAnimationIndex;
			textData.push_back(std::move(t));
		}
		else if (type == "DynamicCGChangeAction")
		{
			const auto& iter = params.find("parameter1");
			if (iter == params.cend())continue;

			animationNames.push_back(iter->second);
			nCurrentAnimationIndex = animationNames.size() - 1;
		}
		else if (type == "InsertDynamicCG")
		{
			const auto& iter = params.find("parameter2");
			if (iter == params.cend())continue;

			animationNames.push_back(iter->second);
			nCurrentAnimationIndex = animationNames.size() - 1;
		}
	}

	return;
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
				"Anus", "Dick", "dick", "Egg", "egg", "Pussy", "pussy", "Toy", "JJ", "mako", "water", "Leg", "Hip"
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
			return strstr(szSlotName, "mm") != nullptr || strstr(szSlotName, "MM") != nullptr;
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
		{"2049_CG2", {"Man_Dick_act1_tr2", "Man_Dick_act2-6", "Pussy_B2", "Pussy_Close_act2", "Pussy_Shadow_act2-6", "Pussy_F2"}},
		{"2057_CG2", {"M_dick2", "Pussy_3"}},
		{"2068_CG2", {"F_omako_4", "M_JJ_3", "M_JJ_4"}}
	};

	const auto fileName = path_utility::ExtractFileName(atlasPath);

	const auto& iter = irrergularMaskSlotMap.find(fileName);
	if (iter != irrergularMaskSlotMap.cend())
	{
		return iter->second;
	}
	return std::vector<std::string>();
}
