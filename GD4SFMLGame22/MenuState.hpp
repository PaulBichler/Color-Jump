#pragma once
#include "State.hpp"
#include "Container.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include "Button.hpp"

class MenuState : public State
{
public:
	void CreateButton(Context context, std::shared_ptr<GUI::Button>& play_button, const int x, const int y, const std::
	                  string& label, const GUI::Button::Callback& callback) const;
	MenuState(StateStack& stack, Context context);
	virtual void Draw();
	virtual bool Update(sf::Time dt);
	virtual bool HandleEvent(const sf::Event& event);

private:
	sf::Sprite m_background_sprite;
	GUI::Container m_gui_container;
};

