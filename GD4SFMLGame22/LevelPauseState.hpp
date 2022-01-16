#pragma once
#include "State.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include "Container.hpp"

class LevelPauseState : public State
{
public:
	LevelPauseState(StateStack& stack, Context context);

	virtual void Draw();
	virtual bool Update(sf::Time dt);
	virtual bool HandleEvent(const sf::Event& event);


private:
	sf::Sprite m_background_sprite;
	sf::Text m_paused_text;
	GUI::Container m_gui_container;
};

