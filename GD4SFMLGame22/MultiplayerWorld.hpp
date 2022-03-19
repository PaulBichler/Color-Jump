#pragma once
#include "World.hpp"

class MultiplayerWorld : public World
{
public:
	MultiplayerWorld(sf::RenderTarget& output_target, SoundPlayer& sounds, LevelManager& level_manager);

	void Update(sf::Time dt) override;
	void Draw() override;
	Character* AddCharacter(sf::Int32 identifier, const sf::Vector2f& position);
	Character* GetCharacter(sf::Int32 identifier) const;
	void RemoveCharacter(sf::Int32 identifier);

private:
	LevelInfo& BuildLevel(LevelManager::LevelData current_level_data) override;

protected:
	void SetCamera() override;
	void HandleCollisions() override;
	sf::FloatRect GetBattlefieldBounds() const override;
	std::vector<Character*> m_players;
};
