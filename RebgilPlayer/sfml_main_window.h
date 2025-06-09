#ifndef SFML_MAIN_WINDOW_H_
#define SFML_MAIN_WINDOW_H_

#include <memory>

#include "sfml_spine_player.h"
/* Microsoft Media Foundation is used because SFML does not provide functionality to play .m4a file. */
#include "mf_media_player.h"
#include "adv.h"

class CSfmlMainWindow
{
public:
	CSfmlMainWindow(const wchar_t *swzWindowName = nullptr);
	~CSfmlMainWindow();

	bool SetSpineFromFile(const std::vector<std::string>& atlasPaths, const std::vector<std::string>& skelPaths, bool bIsBinary);
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
	void SetTexts(const std::vector<adv::TextDatum>& textData);
private:
	sf::Font m_font;
	sf::Text m_msgText;
	sf::Clock m_textClock;

	size_t m_nTextIndex = 0;
	std::vector<adv::TextDatum> m_textData;
	bool m_bTextHidden = false;

	void Redraw();

	void ToggleTextColor();

	void CheckTimer();
	void ShiftMessageText(bool bForward);
	void UpdateMessageText();

	std::unique_ptr<CMfMediaPlayer> m_pAudioPlayer;
	void ChangePlaybackRate(bool bFaster);
	void ResetPlacybackRate();
};

#endif // !SFML_MAIN_WINDOW_H_
