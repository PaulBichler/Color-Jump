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
