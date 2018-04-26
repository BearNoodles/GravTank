#include "Menu.h"



Menu::Menu()
{
}

gef::Sprite Menu::GetSprite()
{
	return m_sprite;
}

void Menu::SetSprite(gef::Sprite sprite)
{
	m_sprite = sprite;
	m_sprite.set_position(-250, -250, 0);
	m_sprite.set_height(500);
	m_sprite.set_width(500);
}



Menu::~Menu()
{
}
