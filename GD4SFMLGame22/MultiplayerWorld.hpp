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
	Character* AddGhostCharacterWithColor(const sf::Int8 identifier, const EColorType color, const sf::IntRect& int_rect, const sf::Vector2f& spawn_pos);
	Character* AddGhostCharacter(const sf::Int8 identifier);
	void UpdatePlatform(sf::Int8 platform_id, const EPlatformType platform_color);
	Character* AddCharacter(const sf::Int8 identifier, const bool is_client_player = false) override;
	Character* GetClientCharacter() const;
	void SetPlatformOnCharacter(Character* character, sf::Int8 platform_id) const;
	void SetTeammate(Character* character);
	Character* GetTeammate() const;
	void UpdatePlatformColors(const std::map<sf::Int8, sf::Int8>& platform_colors) const;
	void UpdateCharacters(sf::Int8 team_id) const;
	void UpdateCharacters() const;

protected:
	void HandleCollisions() override;
	sf::FloatRect GetBattlefieldBounds() const override;
	void DestroyEntitiesOutsideView() override;

private:
	void OnReachedCheckpoint();
	void OnClientPlayerDeath() const;

private:
	Platform* m_checkoint;
	Character* m_client_player;
	Character* m_team_mate;
	std::function<void()> m_lose_callback;

public:
	MultiplayerGameState* m_state;
	std::vector<Platform*> m_platforms;
};