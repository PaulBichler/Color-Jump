#pragma once

#include "Character.hpp"
#include "Platform.hpp"
#include "Tile.hpp"

class TileFactory
{
	TextureHolder& m_textures;
	sf::Vector2u m_tile_size;
	unsigned int m_tile_map_columns;


public:
	PlatformPart* CreatePlatformPart(ETileType tile_type, sf::Vector2f spawn_pos, Platform* parent) const;
	Tile* CreateTile(ETileType tile_type, sf::Vector2f spawn_pos, bool has_collider) const;
	sf::IntRect GetSubRect(ETileType tile_type, sf::Vector2f& spawn_pos) const;
	TileFactory(TextureHolder& textures, sf::Vector2u tile_size);
};
