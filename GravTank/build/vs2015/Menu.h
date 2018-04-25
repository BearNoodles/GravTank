#pragma once
#include "graphics\sprite.h"



class Menu
{
public:
	Menu();
	~Menu();

	void SetSprite(gef::Sprite sprite);
	gef::Sprite GetSprite();

private:


	gef::Sprite m_sprite;
	
};

