#include "MultiplayerWorld.hpp"

MultiplayerWorld::MultiplayerWorld(sf::RenderTarget& output_target, SoundPlayer& sounds, LevelManager& level_manager)
	: World(output_target, sounds, level_manager)
{
}

void MultiplayerWorld::Update(sf::Time dt)
{
	World::Update(dt);
}

void MultiplayerWorld::Draw()
{
	World::Draw();
}

LevelInfo& MultiplayerWorld::BuildLevel(LevelManager::LevelData current_level_data)
{
	LevelInfo info;
	return info;
}

void MultiplayerWorld::SetCamera()
{
}

void MultiplayerWorld::HandleCollisions()
{
}

sf::FloatRect MultiplayerWorld::GetBattlefieldBounds() const
{
	return sf::FloatRect();
}
