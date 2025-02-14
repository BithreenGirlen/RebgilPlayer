

#include "sfml_main_window.h"

CSfmlMainWindow::CSfmlMainWindow(const wchar_t* swzWindowName)
{
	if (swzWindowName != nullptr)m_windowName = swzWindowName;
}

CSfmlMainWindow::~CSfmlMainWindow()
{

}

bool CSfmlMainWindow::SetSpineFromFile(const std::vector<std::string>& atlasPaths, const std::vector<std::string>& skelPaths, bool bIsBinary)
{
	return m_sfmlSpinePlayer.SetSpineFromFile(atlasPaths, skelPaths, bIsBinary);
}

void CSfmlMainWindow::SetSlotsToExclude(const std::vector<std::string>& slotNames)
{
	m_sfmlSpinePlayer.SetSlotsToExclude(slotNames);
}

void CSfmlMainWindow::SetSlotExclusionCallback(bool(*pFunc)(const char*, size_t))
{
	m_sfmlSpinePlayer.SetSlotExclusionCallback(pFunc);
}

int CSfmlMainWindow::Display()
{
	sf::Vector2f fWindowSize = m_sfmlSpinePlayer.GetBaseSize();
	m_window = std::make_unique<sf::RenderWindow>(sf::VideoMode(static_cast<unsigned int>(fWindowSize.x), static_cast<unsigned int>(fWindowSize.y)), m_windowName, sf::Style::None);
	m_window->setPosition(sf::Vector2i(0, 0));
	m_window->setFramerateLimit(0);

	m_sfmlSpinePlayer.SetRenderWindow(m_window.get());
	m_sfmlSpinePlayer.ResetScale();

	UpdateMessageText();

	int iRet = 0;

	sf::Vector2i iMouseStartPos;

	bool bOnWindowMove = false;
	bool bSpeedHavingChanged = false;

	m_spineClock.restart();
	while (m_window->isOpen())
	{
		sf::Event event;
		while (m_window->pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				m_window->close();
				break;
			case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					iMouseStartPos.x = event.mouseButton.x;
					iMouseStartPos.y = event.mouseButton.y;
				}
				break;
			case sf::Event::MouseButtonReleased:
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					if (bSpeedHavingChanged)
					{
						bSpeedHavingChanged = false;
						break;
					}

					if (bOnWindowMove || sf::Mouse::isButtonPressed(sf::Mouse::Right))
					{
						bOnWindowMove ^= true;
						break;
					}

					int iX = iMouseStartPos.x - event.mouseButton.x;
					int iY = iMouseStartPos.y - event.mouseButton.y;

					if (iX == 0 && iY == 0)
					{
						m_sfmlSpinePlayer.ShiftAnimation();
					}
					else
					{
						m_sfmlSpinePlayer.MoveViewPoint(iX, iY);
					}
				}
				if (event.mouseButton.button == sf::Mouse::Middle)
				{
					m_sfmlSpinePlayer.ResetScale();
				}
				break;
			case sf::Event::MouseWheelScrolled:
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
				{
					m_sfmlSpinePlayer.RescaleTime(event.mouseWheelScroll.delta < 0);
					bSpeedHavingChanged = true;
				}
				else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
				{
					ShiftMessageText(event.mouseWheelScroll.delta < 0);
				}
				else
				{
					m_sfmlSpinePlayer.RescaleSkeleton(event.mouseWheelScroll.delta < 0, !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl));
				}
				break;
			case sf::Event::KeyReleased:
				switch (event.key.code)
				{
				case sf::Keyboard::Key::A:
					m_sfmlSpinePlayer.SwitchPma();
					break;
				case sf::Keyboard::Key::B:
					m_sfmlSpinePlayer.SwitchBlendModeAdoption();
					break;
				case sf::Keyboard::Key::C:
					SwitchTextColor();
					break;
				case sf::Keyboard::Key::R:
					m_sfmlSpinePlayer.SwitchDrawOrder();
					break;
				case sf::Keyboard::Key::S:
					SaveCurrentFrameImage();
					break;
				case sf::Keyboard::Key::T:
					m_bTextHidden ^= true;
					break;
				case sf::Keyboard::Key::Escape:
					m_window->close();
					break;
				case sf::Keyboard::Key::PageUp:
					ChangePlaybackRate(true);
					break;
				case sf::Keyboard::Key::PageDown:
					ChangePlaybackRate(false);
					break;
				case sf::Keyboard::Key::Home:
					ResetPlacybackRate();
					break;
				case sf::Keyboard::Key::Up:
					iRet = 2;
					m_window->close();
					break;
				case sf::Keyboard::Key::Down:
					iRet = 1;
					m_window->close();
					break;
				default:
					break;
				}
				break;
			}
		}

		Redraw();

		CheckTimer();

		if (bOnWindowMove)
		{
			int iPosX = sf::Mouse::getPosition().x - m_window->getSize().x / 2;
			int iPosY = sf::Mouse::getPosition().y - m_window->getSize().y / 2;
			m_window->setPosition(sf::Vector2i(iPosX, iPosY));
		}
	}


	return iRet;
}

bool CSfmlMainWindow::SaveCurrentFrameImage()
{
	/*SFML does not have API equivalent to SDL's SDL_GetBasePath().*/
	std::string strStrFilePath = m_sfmlSpinePlayer.GetCurrentAnimationNameWithTrackTime();
	if (strStrFilePath.empty())return false;
	strStrFilePath += ".png";

	sf::Texture texture;
	bool bRet = texture.create(m_window->getSize().x, m_window->getSize().y);
	if (!bRet)return false;

	texture.update(*m_window);
	sf::Image image = texture.copyToImage();

	bRet = image.saveToFile(strStrFilePath);
	
	return bRet;
}

bool CSfmlMainWindow::SetFont(const std::string& strFilePath, bool bBold, bool bItalic)
{
	bool bRet = m_font.loadFromFile(strFilePath);
	if (!bRet)return false;

	constexpr float fOutLineThickness = 2.4f;

	m_msgText.setFont(m_font);
	m_msgText.setFillColor(sf::Color::Black);
	m_msgText.setStyle((bBold ? sf::Text::Style::Bold : 0) | (bItalic ? sf::Text::Style::Italic : 0));
	m_msgText.setOutlineThickness(fOutLineThickness);
	m_msgText.setOutlineColor(sf::Color::White);

	return true;
}
/*文章格納*/
void CSfmlMainWindow::SetTexts(const std::vector<adv::TextDatum>& textData)
{
	m_textData = textData;
	m_nTextIndex = 0;
	m_msgText.setString("");

	const auto HasAudio = [](const adv::TextDatum& text)
		-> bool
		{
			return !text.wstrVoicePath.empty();
		};
	const auto iter = std::find_if(m_textData.begin(), m_textData.end(), HasAudio);
	if (iter != m_textData.cend())
	{
		m_pAudioPlayer = std::make_unique<CMfMediaPlayer>();
	}
}
/*再描画*/
void CSfmlMainWindow::Redraw()
{
	float fDelta = m_spineClock.getElapsedTime().asSeconds();
	m_spineClock.restart();

	if (m_window.get() != nullptr)
	{
		m_window->clear(sf::Color(0, 0, 0, 0));

		m_sfmlSpinePlayer.Redraw(fDelta);

		if (!m_bTextHidden)
		{
			m_window->draw(m_msgText);
		}

		m_window->display();
	}
}
/*文字色切り替え*/
void CSfmlMainWindow::SwitchTextColor()
{
	m_msgText.setFillColor(m_msgText.getFillColor() == sf::Color::Black ? sf::Color::White : sf::Color::Black);
	m_msgText.setOutlineColor(m_msgText.getFillColor() == sf::Color::Black ? sf::Color::White : sf::Color::Black);
}
/*文章切り替え是否確認*/
void CSfmlMainWindow::CheckTimer()
{
	constexpr float fAutoPlayInterval = 2.f;
	float fSecond = m_textClock.getElapsedTime().asSeconds();
	if (m_pAudioPlayer.get() != nullptr && m_pAudioPlayer.get()->IsEnded() && fSecond > fAutoPlayInterval)
	{
		if (m_nTextIndex < m_textData.size() - 1)
		{
			ShiftMessageText(true);
		}
		else
		{
			m_textClock.restart();
		}
	}
}
/*表示文章移行*/
void CSfmlMainWindow::ShiftMessageText(bool bForward)
{
	if (bForward)
	{
		++m_nTextIndex;
		if (m_nTextIndex >= m_textData.size())m_nTextIndex = 0;
	}
	else
	{
		--m_nTextIndex;
		if (m_nTextIndex >= m_textData.size())m_nTextIndex = m_textData.size() - 1;
	}
	UpdateMessageText();
}
/*表示文章変更適用*/
void CSfmlMainWindow::UpdateMessageText()
{
	if (m_nTextIndex >= m_textData.size())return;

	const adv::TextDatum& textDatum = m_textData[m_nTextIndex];
	std::wstring wstr = textDatum.wstrText;
	if (!wstr.empty() && wstr.back() != L'\n') wstr += L"\n ";

	wstr += std::to_wstring(m_nTextIndex + 1) + L"/" + std::to_wstring(m_textData.size());
	m_msgText.setString(wstr);

	if (!textDatum.wstrVoicePath.empty())
	{
		if (m_pAudioPlayer.get() != nullptr)
		{
			m_pAudioPlayer->Play(textDatum.wstrVoicePath.c_str());
		}
	}
	m_textClock.restart();
}
/*音声再生速度変更*/
void CSfmlMainWindow::ChangePlaybackRate(bool bFaster)
{
	if (m_pAudioPlayer.get() != nullptr)
	{
		double dbRate = m_pAudioPlayer.get()->GetCurrentRate();
		constexpr double fRatePortion = 0.1;
		if (bFaster && dbRate < 2.49)
		{
			dbRate += fRatePortion;
		}
		else if (!bFaster && dbRate > 0.51)
		{
			dbRate -= fRatePortion;

		}
		m_pAudioPlayer.get()->SetCurrentRate(dbRate);
	}
}
/*音声再生速度初期化*/
void CSfmlMainWindow::ResetPlacybackRate()
{
	if (m_pAudioPlayer.get() != nullptr)
	{
		m_pAudioPlayer.get()->SetCurrentRate(1.0);
	}
}
