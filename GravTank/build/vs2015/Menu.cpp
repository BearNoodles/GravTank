#include "Menu.h"



Menu::Menu(int width, int height) :
	m_width(width),
	m_height(height)
{
}

gef::Sprite Menu::GetSprite()
{
	return m_sprite;
}

//Set up sprite for the menu screen
void Menu::SetSprite(gef::Sprite sprite)
{
	m_sprite = sprite;
	m_sprite.set_height(m_height);
	m_sprite.set_width(m_width);
	m_sprite.set_position(m_width / 2, m_height / 2, 0);
	
}



Menu::~Menu()
{
}
