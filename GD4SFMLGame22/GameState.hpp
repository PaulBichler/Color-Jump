#pragma once
#include "SinglePlayerWorld.hpp"
#include "State.hpp"
#include "World.hpp"

class GameState : public State
{
public:
	GameState(StateStack& stack, Context context);
	void Draw() override;
	bool Update(sf::Time dt) override;
	bool HandleEvent(const sf::Event& event) override;

private:
	SinglePlayerWorld m_world;
	Player m_player;
};

