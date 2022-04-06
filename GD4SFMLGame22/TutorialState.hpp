#pragma once
#include <SFML/Graphics/Sprite.hpp>

#include "Container.hpp"
#include "State.hpp"

class TutorialState : public State
{
public:
	TutorialState(StateStack& stack, Context& context);

	void Draw() override;
	bool Update(sf::Time dt) override;
	bool HandleEvent(const sf::Event& event) override;

private:
	GUI::Container m_gui_container;
	sf::Sprite m_tutorial1;
	sf::Sprite m_tutorial2;
	sf::Sprite m_tutorial3;
};

