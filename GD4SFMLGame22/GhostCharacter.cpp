#include "GhostCharacter.hpp"

void GhostCharacter::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{
}

sf::FloatRect GhostCharacter::GetBoundingRect() const
{
	return { 0,0,0,0 };
}

unsigned GhostCharacter::GetCategory() const
{
    return Category::kGhost;
}