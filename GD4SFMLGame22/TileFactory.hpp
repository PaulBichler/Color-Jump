#pragma once
#include <SFML/Graphics/Texture.hpp>

#include "Character.hpp"
#include "ETileType.hpp"
#include "SpriteNode.hpp"
#include "Tile.hpp"

class TileFactory
{
private:
	TextureHolder& m_textures;
	sf::Vector2u m_tile_size;
	unsigned int m_tile_map_columns;

	sf::IntRect GetSubRect(int pos) const;

public:
	TileFactory(TextureHolder& textures, sf::Vector2u tile_size);

	Tile* CreateTile(int pos, sf::Vector2f spawn_pos) const;
	Character* CreatePlayer(int id, ECharacterType type, sf::Vector2f spawn_pos) const;
};

