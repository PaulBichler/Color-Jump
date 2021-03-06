#pragma once
#include <SFML/Graphics/Sprite.hpp>

#include "Button.hpp"
#include "Container.hpp"
#include "Label.hpp"
#include "State.hpp"

class JoinServerState : public State
{
public:
	JoinServerState(StateStack& stack, Context& context);
	void Draw() override;
	bool Update(sf::Time dt) override;
	bool HandleEvent(const sf::Event& event) override;

private:
	sf::Sprite m_background_sprite;
	GUI::Container m_gui_container;

	GUI::Button::Ptr m_change_ip_button;
	GUI::Label::Ptr m_current_ip_label;
	std::string m_player_input_ip;
};

