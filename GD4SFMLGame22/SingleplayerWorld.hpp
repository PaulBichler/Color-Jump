#pragma once
#include "World.hpp"

class SingleplayerWorld :public World
{
public:
	SingleplayerWorld(sf::RenderTarget& output_target, SoundPlayer& sounds, LevelManager& level_manager);

	void Update(sf::Time dt) override;
	void Draw() override;

	void SetLoseCallback(const std::function<void()>& callback);
	void SetWinCallback(const std::function<void()>& callback);

protected:
	void DestroyEntitiesOutsideView();
	void SetCamera() override;
	sf::FloatRect GetBattlefieldBounds() const override;
	void HandleCollisions() override;

private:
	std::function<void()> m_lose_callback;
	std::function<void()> m_win_callback;
	bool m_has_won{};
};

