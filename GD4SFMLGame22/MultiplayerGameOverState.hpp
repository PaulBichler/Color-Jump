#pragma once
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include "Container.hpp"
#include "Label.hpp"
#include "State.hpp"

class MultiplayerGameOverState : public State
{
public:
	MultiplayerGameOverState(StateStack& stack, Context& context, bool has_won);

	void Draw() override;
	bool Update(sf::Time dt) override;
	bool HandleEvent(const sf::Event& event) override;

private:
	void UpdateLeaderboard() const;

private:
	sf::Sprite m_background_sprite;
	sf::Text m_title_text;
	GUI::Container m_gui_container;
	std::vector<GUI::Label*> m_leaderboard;
};
