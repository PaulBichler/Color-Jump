#pragma once
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include "Container.hpp"
#include "State.hpp"

class LevelLoseState : public State
{
public:
	LevelLoseState(StateStack& stack, Context& context);

	void Draw() override;
	bool Update(sf::Time dt) override;
	bool HandleEvent(const sf::Event& event) override;


private:
	sf::Sprite m_background_sprite;
	sf::Text m_lost_text;
	GUI::Container m_gui_container;
};

