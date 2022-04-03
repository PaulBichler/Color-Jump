//Written by Paul Bichler (D00242563)

#include "Tile.hpp"

#include "EPlatformType.hpp"
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"

//Written by Paul Bichler (D00242563)
//The Tile class is the base class of all tiles in the level
Tile::Tile(const TextureHolder& textures, const sf::IntRect sub_rect, const ETileType type)
	: SpriteNode(textures.Get(Textures::kLevelTileSet), sub_rect),
	  m_type(type)
{
}

//Written by Paul Bichler (D00242563)
ETileType Tile::GetType() const
{
	return m_type;
}

//Written by Paul Bichler (D00242563)
//Not all tiles need collision detection (tiles on the background layer for example)
void Tile::SetActiveCollider(bool active)
{
	m_has_collider = active;
}

//Written by Paul Bichler (D00242563)
//Returns the right category based on the tile type
unsigned Tile::GetCategory() const
{
	//Return the default category for non-collider tiles
	if (!m_has_collider)
		return Category::kPlatform;

	switch (m_type)
	{
	case kSpikes:
		return Category::kEnemyTrap;
	default:
		return Category::kPlatform;
	}
}

//Written by Paul Bichler (D00242563)
sf::FloatRect Tile::GetBoundingRect() const
{
	if (!m_has_collider)
		return SpriteNode::GetBoundingRect();

	return GetWorldTransform().transformRect(m_sprite.getGlobalBounds());
}

bool Tile::HandleCollision(const EColorType color)
{
	switch (m_type)
	{
	case kHorizontalBluePlatformPart:
	case kVerticalBluePlatformPart:
		if (color != EColorType::kBlue)
			return false;
		break;
	case kHorizontalRedPlatformPart:
	case kVerticalRedPlatformPart:
		if (color != EColorType::kRed)
			return false;
		break;
	default:
		break;
	}

	return true;
}

sf::IntRect Tile::GetSize() const
{
	return m_sprite.getTextureRect();
}
