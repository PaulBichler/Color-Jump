#pragma once

#include "Character.hpp"
#include "ColorTile.hpp"
#include "Platform.hpp"
#include "Tile.hpp"

class TileFactory
{
	TextureHolder& m_textures;
	FontHolder& m_fonts;
	sf::Vector2u m_tile_size;
	unsigned int m_tile_map_columns;
	SoundPlayer& m_sound_player;


public:
	TileFactory(TextureHolder& textures, FontHolder& fonts, sf::Vector2u tile_size, SoundPlayer& sound_player);
	PlatformPart* CreatePlatformPart(ETileType tile_type, sf::Vector2f spawn_pos,
	                                 Platform* parent) const;
	Tile* CreateTile(ETileType tile_type, sf::Vector2f spawn_pos, bool has_collider) const;
	ColorTile* CreateColorTile(ETileType tile_type, EColorType color_type,
	                           sf::Vector2f spawn_pos) const;
	Character* CreatePlayer(ETileType tile_type, EColorType type, sf::Vector2f spawn_pos) const;

	sf::IntRect GetSubRect(ETileType tile_type, sf::Vector2f& spawn_pos) const;
};
