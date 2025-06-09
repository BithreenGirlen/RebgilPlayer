﻿#ifndef SFML_MAIN_WINDOW_H_
#define SFML_MAIN_WINDOW_H_

#include <memory>

#include "sfml_spine_player.h"

#include "adv.h"
/* Microsoft Media Foundation is used because SFML does not provide functionality to play .m4a file. */
#include "mf_media_player.h"

class CSfmlMainWindow
{
public:
	CSfmlMainWindow(const wchar_t* swzWindowName = nullptr);
	~CSfmlMainWindow();

	bool SetSpineFromFile(const std::vector<std::string>& atlasPaths, const std::vector<std::string>& skelPaths, bool isBinarySkel);

	void SetSlotsToExclude(const std::vector<std::string>& slotNames);
	void SetSlotExclusionCallback(bool (*pFunc)(const char*, size_t));

	int Display();
private:
	std::unique_ptr<sf::RenderWindow> m_window;

	std::unique_ptr<CSfmlSpinePlayer> m_sfmlSpinePlayer;
	sf::Clock m_spineClock;

	void ResizeWindow();

	bool SaveCurrentFrameImage();

	/*文章表示用*/
public:
	bool SetFont(const std::string& strFilePath, bool bBold = true, bool bItalic = true);
	void SetScenarioData(std::vector<adv::TextDatum>& textData, std::vector<std::string>& animationNames);
private:
	sf::Font m_font;
	sf::Text m_msgText;
	sf::Clock m_textClock;

	std::vector<adv::TextDatum> m_textData;
	size_t m_nTextIndex = 0;
	bool m_bTextHidden = false;

	std::vector<std::string> m_animationNames;
	size_t m_nLastAnimationIndex = 0;

	void ToggleTextColor();
	void ToggleTextVisibility();

	void CheckTimer();
	void ShiftMessageText(bool bForward);
	void UpdateMessageText();
	void AdjustTextBounds();

	std::unique_ptr<CMfMediaPlayer> m_pAudioPlayer;
};

#endif // !SFML_MAIN_WINDOW_H_
