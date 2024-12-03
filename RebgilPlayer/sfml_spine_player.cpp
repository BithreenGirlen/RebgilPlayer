

#include "sfml_spine_player.h"

CSfmlSpinePlayer::CSfmlSpinePlayer()
{

}

CSfmlSpinePlayer::~CSfmlSpinePlayer()
{

}

void CSfmlSpinePlayer::SetRenderWindow(sf::RenderWindow* pSfmlWindow)
{
	m_pSfmlWindow = pSfmlWindow;
}

void CSfmlSpinePlayer::Redraw(float fDelta)
{
	if (m_pSfmlWindow != nullptr)
	{
		if (!m_bDrawOrderReversed)
		{
			for (size_t i = 0; i < m_drawables.size(); ++i)
			{
				m_drawables.at(i)->Update(fDelta);
				m_pSfmlWindow->draw(*m_drawables.at(i).get(), sf::RenderStates(sf::BlendAlpha));
			}
		}
		else
		{
			for(long long i = m_drawables.size() - 1;i >= 0;--i)
			{
				m_drawables.at(i)->Update(fDelta);
				m_pSfmlWindow->draw(*m_drawables.at(i).get(), sf::RenderStates(sf::BlendAlpha));
			}
		}

	}
}

void CSfmlSpinePlayer::WorkOutDefaultScale()
{
	unsigned int uiSkeletonWidth = static_cast<unsigned int>(m_fBaseSize.x);
	unsigned int uiSkeletonHeight = static_cast<unsigned int>(m_fBaseSize.y);

	unsigned int uiDesktopWidth = sf::VideoMode::getDesktopMode().width;
	unsigned int uiDesktopHeight = sf::VideoMode::getDesktopMode().height;

	if (uiSkeletonWidth > uiDesktopWidth || uiSkeletonHeight > uiDesktopHeight)
	{
		if (uiDesktopWidth > uiDesktopHeight)
		{
			m_fDefaultScale = static_cast<float>(uiDesktopHeight) / uiSkeletonHeight;
			m_fThresholdScale = static_cast<float>(uiDesktopWidth) / uiSkeletonWidth;
		}
		else
		{
			m_fDefaultScale = static_cast<float>(uiDesktopWidth) / uiSkeletonWidth;
			m_fThresholdScale = static_cast<float>(uiDesktopHeight) / uiSkeletonHeight;
		}
		m_fSkeletonScale = m_fDefaultScale;
	}
}

void CSfmlSpinePlayer::AdjustViewOffset()
{
	/*Nothing to do.*/
}

void CSfmlSpinePlayer::ResizeWindow()
{
	if (m_pSfmlWindow == nullptr)return;
	/*Actually, these should be procedures for RescaleSkeleton()*/
#ifdef SFML_SPINE_CPP
	float fOffset = m_fSkeletonScale - m_fThresholdScale > 0.f ? m_fSkeletonScale - m_fThresholdScale : 0;
	for (size_t i = 0; i < m_drawables.size(); ++i)
	{
		m_drawables.at(i).get()->skeleton->setScaleX(m_fSkeletonScale > 0.99f + fOffset ? m_fSkeletonScale : 1.f + fOffset);
		m_drawables.at(i).get()->skeleton->setScaleY(m_fSkeletonScale > 0.99f + fOffset ? m_fSkeletonScale : 1.f + fOffset);
	}

	unsigned int uiWindowWidthMax = static_cast<unsigned int>(m_fBaseSize.x * (m_fSkeletonScale - m_kfScalePortion));
	unsigned int uiWindowHeightMax = static_cast<unsigned int>(m_fBaseSize.y * (m_fSkeletonScale - m_kfScalePortion));
	if (uiWindowWidthMax < sf::VideoMode::getDesktopMode().width || uiWindowHeightMax < sf::VideoMode::getDesktopMode().height)
	{
		m_pSfmlWindow->setSize(sf::Vector2u(static_cast<unsigned int>(m_fBaseSize.x * m_fSkeletonScale), static_cast<unsigned int>(m_fBaseSize.y * m_fSkeletonScale)));
	}
#elif SFML_SPINE_C

#endif

	m_pSfmlWindow->setSize(sf::Vector2u(static_cast<unsigned int>(m_fBaseSize.x * m_fSkeletonScale), static_cast<unsigned int>(m_fBaseSize.y * m_fSkeletonScale)));
}
