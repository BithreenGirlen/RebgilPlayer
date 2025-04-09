#ifndef SFML_SPINE_PLAYER_H_
#define SFML_SPINE_PLAYER_H_

#include"spine_player.h"

class CSfmlSpinePlayer : public CSpinePlayer
{
public:
	CSfmlSpinePlayer(sf::RenderWindow* pSfmlWindow);
	~CSfmlSpinePlayer();

	virtual void Redraw(float fDelta);

	void ResizeWindow();
private:
	virtual void WorkOutDefaultScale();

	float m_fThresholdScale = 1.f;

	sf::RenderWindow *m_pSfmlWindow = nullptr;
};

#endif // !SFML_SPINE_PLAYER_H_
