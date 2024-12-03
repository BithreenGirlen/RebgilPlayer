#ifndef SFML_SPINE_PLAYER_H_
#define SFML_SPINE_PLAYER_H_

#include"spine_player.h"

class CSfmlSpinePlayer : public CSpinePlayer
{
public:
	CSfmlSpinePlayer();
	~CSfmlSpinePlayer();

	void SetRenderWindow(sf::RenderWindow* pSfmlWindow);

	virtual void Redraw(float fDelta);

	sf::Vector2f GetBaseSize() const { return m_fBaseSize; }
private:
	virtual void WorkOutDefaultScale();
	virtual void AdjustViewOffset();
	virtual void ResizeWindow();

	float m_fThresholdScale = 1.f;

	sf::RenderWindow *m_pSfmlWindow = nullptr;
};

#endif // !SFML_SPINE_PLAYER_H_
