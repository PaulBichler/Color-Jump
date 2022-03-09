#include "MultiplayerWorld.hpp"

MultiplayerWorld::MultiplayerWorld(sf::RenderTarget& output_target, SoundPlayer& sounds, LevelManager& level_manager)
	: World(output_target, sounds, level_manager)
{
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
