#pragma once
class MultiplayerGameState;
#include "World.hpp"

class MultiplayerWorld : public World
{
public:
	MultiplayerWorld(sf::RenderTarget& output_target, SoundPlayer& sounds, FontHolder& fonts, MultiplayerGameState* state);

	void Update(sf::Time dt) override;
	void Draw() override;
	Character* GetCharacter(sf::Int32 identifier) const;
	void RemoveCharacter(sf::Int32 identifier);
	Character* AddGhostCharacterWithColor(sf::Int32 identifier, EColorType color, const sf::IntRect& int_rect, const sf::Vector2f& spawn_pos);
	Character* AddGhostCharacter(sf::Int32 identifier);
	void UpdatePlatform(sf::Int32 platform_id, EPlatformType platform_color) const;
	Character* AddCharacter(sf::Int32 identifier, bool is_client_player = false) override;

protected:
	void SetCamera() override;
	void HandleCollisions() override;
	sf::FloatRect GetBattlefieldBounds() const override;

private:
	Character* m_client_player;
	std::function<void()> m_lose_callback;
	std::function<void()> m_reached_goal_callback;

public:
	MultiplayerGameState* m_state;
	std::vector<Platform*> m_platforms;
};
