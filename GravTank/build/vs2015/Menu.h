#pragma once
#include "graphics\sprite.h"



class Menu
{
public:
	Menu(int width, int height);
	~Menu();

	void SetSprite(gef::Sprite sprite);
	gef::Sprite GetSprite();

private:

	gef::Sprite m_sprite;
	int m_width;
	int m_height;
	
};

