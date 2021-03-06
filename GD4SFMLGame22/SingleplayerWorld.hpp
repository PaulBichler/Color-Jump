#pragma once
#include "World.hpp"

class SinglePlayerWorld : public World
{
public:
	SinglePlayerWorld(sf::RenderTarget& output_target, SoundPlayer& sounds, FontHolder& fonts);

	void Update(sf::Time dt) override;
	void Draw() override;

	void SetLoseCallback(const std::function<void()>& callback);
	void SetWinCallback(const std::function<void()>& callback);

protected:
	void UpdateSounds() const;
	void DestroyEntitiesOutsideView() override;
	sf::FloatRect GetBattlefieldBounds() const override;
	void HandleCollisions(sf::Time dt) override;

private:
	std::function<void()> m_lose_callback;
	std::function<void()> m_win_callback;
};

