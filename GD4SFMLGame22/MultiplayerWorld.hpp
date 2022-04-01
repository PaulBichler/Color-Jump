#pragma once
#include "World.hpp"

class MultiplayerWorld : public World
{
public:
	MultiplayerWorld(sf::RenderTarget& output_target, SoundPlayer& sounds);

	void Update(sf::Time dt) override;
	void Draw() override;
	Character* GetCharacter(sf::Int32 identifier) const;
	void RemoveCharacter(sf::Int32 identifier);

protected:
	void SetCamera() override;
	void HandleCollisions() override;
	sf::FloatRect GetBattlefieldBounds() const override;

private:
	std::function<void()> m_lose_callback;
	std::function<void()> m_reached_goal_callback;
};
