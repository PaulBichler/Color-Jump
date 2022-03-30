#pragma once
#include "SingleplayerLevelLoader.hpp"
#include "World.hpp"

class SinglePlayerWorld : public World
{
public:
	SinglePlayerWorld(sf::RenderTarget& output_target, SoundPlayer& sounds);

	void Update(sf::Time dt) override;
	void Draw() override;

	void SetLoseCallback(const std::function<void()>& callback);
	void SetWinCallback(const std::function<void()>& callback);

protected:
	LevelInfo& BuildLevel(LevelManager::LevelData current_level_data) override;
	void UpdateSounds() const;
	void UpdatePlatforms(sf::Time dt) const;
	void DestroyEntitiesOutsideView();
	void SetCamera() override;
	sf::FloatRect GetBattlefieldBounds() const override;
	void HandleCollisions() override;

private:
	std::function<void()> m_lose_callback;
	std::function<void()> m_win_callback;
	bool m_has_won{};

	SLevelInfo m_s_level_info;
};

