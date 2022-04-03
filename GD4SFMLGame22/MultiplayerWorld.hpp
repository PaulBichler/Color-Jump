#pragma once
class MultiplayerGameState;
#include "World.hpp"

class MultiplayerWorld : public World
{
public:
	MultiplayerWorld(sf::RenderTarget& output_target, SoundPlayer& sounds, FontHolder& fonts, MultiplayerGameState* state);

	void Update(sf::Time dt) override;
	void Draw() override;
	Character* GetCharacter(const sf::Int8 identifier) const;
	void RemoveCharacter(const sf::Int8 identifier);
	Character* AddGhostCharacterWithColor(const sf::Int8 identifier, const EColorType color, const sf::IntRect& int_rect, const sf::Vector2f&
	                                      spawn_pos);
	Character* AddGhostCharacter(const sf::Int8 identifier);
	void UpdatePlatform(const sf::Int8 platform_id, const EPlatformType platform_color) const;
	Character* AddCharacter(const sf::Int8 identifier, const bool is_client_player = false) override;

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
