

#include "sfml_spine_player.h"

CSfmlSpinePlayer::CSfmlSpinePlayer(sf::RenderWindow* pSfmlWindow)
	:m_pSfmlWindow(pSfmlWindow)
{

}

CSfmlSpinePlayer::~CSfmlSpinePlayer()
{

}


void CSfmlSpinePlayer::Redraw(float fDelta)
{
	if (m_pSfmlWindow != nullptr)
	{
		if (!m_bDrawOrderReversed)
		{
			for (size_t i = 0; i < m_drawables.size(); ++i)
			{
				m_drawables[i]->Update(fDelta);
				m_pSfmlWindow->draw(*m_drawables[i], sf::RenderStates(sf::BlendAlpha));
			}
		}
		else
		{
			for(long long i = m_drawables.size() - 1;i >= 0;--i)
			{
				m_drawables[i]->Update(fDelta);
				m_pSfmlWindow->draw(*m_drawables[i], sf::RenderStates(sf::BlendAlpha));
			}
		}

	}
}

void CSfmlSpinePlayer::ResizeWindow()
{
	if (m_pSfmlWindow == nullptr)return;
	/* Keep sf::View unchanged. */
#ifdef SFML_SPINE_CPP
	float fOffset = m_fCanvasScale - m_fThresholdScale > 0.f ? m_fCanvasScale - m_fThresholdScale : 0;
	for (const auto& drawable : m_drawables)
	{
		drawable->skeleton->setScaleX(m_fCanvasScale > 0.99f + fOffset ? m_fCanvasScale : 1.f + fOffset);
		drawable->skeleton->setScaleY(m_fCanvasScale > 0.99f + fOffset ? m_fCanvasScale : 1.f + fOffset);
	}

	unsigned int uiWindowWidthMax = static_cast<unsigned int>(m_fBaseSize.x * (m_fCanvasScale - kfScalePortion));
	unsigned int uiWindowHeightMax = static_cast<unsigned int>(m_fBaseSize.y * (m_fCanvasScale - kfScalePortion));
	if (uiWindowWidthMax < sf::VideoMode::getDesktopMode().width || uiWindowHeightMax < sf::VideoMode::getDesktopMode().height)
	{
		m_pSfmlWindow->setSize(sf::Vector2u(static_cast<unsigned int>(m_fBaseSize.x * m_fCanvasScale), static_cast<unsigned int>(m_fBaseSize.y * m_fCanvasScale)));
	}
#elif SFML_SPINE_C
	m_pSfmlWindow->setSize(sf::Vector2u(static_cast<unsigned int>(m_fBaseSize.x * m_fCanvasScale), static_cast<unsigned int>(m_fBaseSize.y * m_fCanvasScale)));
#endif
}
/*標準尺度算出*/
void CSfmlSpinePlayer::WorkOutDefaultScale()
{
	m_fDefaultScale = 1.f;
	m_fDefaultOffset = sf::Vector2f();

	unsigned int uiSkeletonWidth = static_cast<unsigned int>(m_fBaseSize.x);
	unsigned int uiSkeletonHeight = static_cast<unsigned int>(m_fBaseSize.y);

	unsigned int uiDesktopWidth = sf::VideoMode::getDesktopMode().width;
	unsigned int uiDesktopHeight = sf::VideoMode::getDesktopMode().height;

	if (uiSkeletonWidth > uiDesktopWidth || uiSkeletonHeight > uiDesktopHeight)
	{
		float fScaleX = static_cast<float>(uiDesktopWidth) / uiSkeletonWidth;
		float fScaleY = static_cast<float>(uiDesktopHeight) / uiSkeletonHeight;

		if (uiDesktopWidth > uiDesktopHeight)
		{
			m_fDefaultScale = static_cast<float>(uiDesktopHeight) / uiSkeletonHeight;
			m_fThresholdScale = static_cast<float>(uiDesktopWidth) / uiSkeletonWidth;

			m_fDefaultOffset.x = uiSkeletonWidth > uiDesktopWidth ? (uiSkeletonWidth * (1 - fScaleY)) / 2.f : 0.f;
			m_fDefaultOffset.y = uiSkeletonHeight > uiDesktopHeight ? (uiSkeletonHeight - uiDesktopHeight) / 2.f : 0.f;
		}
		else
		{
			m_fDefaultScale = static_cast<float>(uiDesktopWidth) / uiSkeletonWidth;
			m_fThresholdScale = static_cast<float>(uiDesktopHeight) / uiSkeletonHeight;

			m_fDefaultOffset.x = uiSkeletonWidth > uiDesktopWidth ? (uiSkeletonWidth - uiDesktopWidth) / 2.f : 0.f;
			m_fDefaultOffset.y = uiSkeletonHeight > uiDesktopHeight ? (uiSkeletonHeight * (1 - fScaleX)) / 2.f : 0.f;
		}
		m_fSkeletonScale = m_fDefaultScale;
	}
}
