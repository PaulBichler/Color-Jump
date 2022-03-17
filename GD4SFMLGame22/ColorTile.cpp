#include "ColorTile.hpp"

ColorTile::ColorTile(const TextureHolder& textures, const sf::IntRect sub_rect, const ETileType tile_type)
	: Tile(textures, sub_rect, tile_type)
{
}

ETileColor ColorTile::GetColor() const
{
	return m_color;
}

