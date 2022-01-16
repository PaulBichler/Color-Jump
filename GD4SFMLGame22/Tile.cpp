//Written by Paul Bichler (D00242563)

#include "Tile.hpp"

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
	if(!m_has_collider)
		return SpriteNode::GetCategory();

	switch (m_type)
	{
	case kSpikes:
		return Category::kEnemyTrap;
	default:
		return SpriteNode::GetCategory();
	}
}

//Written by Paul Bichler (D00242563)
sf::FloatRect Tile::GetBoundingRect() const
{
	if(!m_has_collider)
		return SpriteNode::GetBoundingRect();

	return GetWorldTransform().transformRect(m_sprite.getGlobalBounds());
}
