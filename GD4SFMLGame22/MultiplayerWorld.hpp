#pragma once
#include "MultiplayerLevelLoader.hpp"
#include "World.hpp"

class MultiplayerWorld : public World
{
public:
	MultiplayerWorld(sf::RenderTarget& output_target, SoundPlayer& sounds);

	void Update(sf::Time dt) override;
	void Draw() override;
	Character* AddCharacterWithColor(const sf::Int32 identifier, const EColorType color, const sf::IntRect rect, const sf::Vector2f
	                                 spawn_pos);
	Character* AddCharacter(sf::Int32 identifier);
	Character* GetCharacter(sf::Int32 identifier) const;
	void RemoveCharacter(sf::Int32 identifier);

private:
	LevelInfo& BuildLevel(LevelManager::LevelData current_level_data) override;

protected:
	void SetCamera() override;
	void HandleCollisions() override;
	sf::FloatRect GetBattlefieldBounds() const override;
	std::vector<Character*> m_players;

private:
	MLevelInfo m_level_info;
};
