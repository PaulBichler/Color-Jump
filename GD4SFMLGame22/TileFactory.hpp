#pragma once

#include "Character.hpp"
#include "Platform.hpp"
#include "SpriteNode.hpp"
#include "Tile.hpp"

class TileFactory
{
private:
	TextureHolder& m_textures;
	sf::Vector2u m_tile_size;
	unsigned int m_tile_map_columns;
	SoundPlayer& m_soundPlayer;

	sf::IntRect GetSubRect(int pos, ETileType tile_type, sf::Vector2f& spawn_pos) const;
	

public:
	TileFactory(TextureHolder& textures, sf::Vector2u tile_size, SoundPlayer& sound_player);
	PlatformPart* CreatePlatformPart(int pos, sf::Vector2f spawn_pos, Platform* parent, ETileType tile_type) const;
	Tile* CreateTile(int pos, sf::Vector2f spawn_pos, ETileType tile_type, bool has_collider) const;
	Character* CreatePlayer(const int id, const ETileType tile_type, const ECharacterType type, sf::Vector2f spawn_pos) const;
};

