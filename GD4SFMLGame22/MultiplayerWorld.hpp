#pragma once
#include "World.hpp"

class MultiplayerWorld : public World
{
public:
	MultiplayerWorld(sf::RenderTarget& output_target, SoundPlayer& sounds, LevelManager& level_manager);

	void Update(sf::Time dt) override;
	void Draw() override;

private:
	LevelInfo& BuildLevel(LevelManager::LevelData current_level_data) override;

protected:
	void SetCamera() override;
	void HandleCollisions() override;
	sf::FloatRect GetBattlefieldBounds() const override;
};

