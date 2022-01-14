#include "Tile.hpp"

#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"

Tile::Tile(const TextureHolder& textures, const sf::IntRect sub_rect, const ETileType type)
	: SpriteNode(textures.Get(Textures::kLevelTileSet), sub_rect),
	m_type(type)
{
}

ETileType Tile::GetType() const
{
	return m_type;
}

void Tile::SetActiveCollider(bool active)
{
	m_has_collider = active;
}

unsigned Tile::GetCategory() const
{
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

sf::FloatRect Tile::GetBoundingRect() const
{
	if(!m_has_collider)
		return SpriteNode::GetBoundingRect();

	return GetWorldTransform().transformRect(m_sprite.getGlobalBounds());
}
