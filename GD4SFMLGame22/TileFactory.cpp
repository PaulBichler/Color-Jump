//Written by Paul Bichler (D00242563)

#include "TileFactory.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "PlatformPart.hpp"
#include "ResourceHolder.hpp"
#include "Tile.hpp"

//Written by Paul Bichler (D00242563)
//Gets the sub rect of the tile type based on X and Y coordinates of the type value.
//The method also adjusts the sub rect to adjust the collider.
sf::IntRect TileFactory::GetSubRect(ETileType tile_type, sf::Vector2f& spawn_pos) const
{
	//The same could be achieved by looping, this is however much simpler.
	//Got the answer from here: https://stackoverflow.com/questions/52825969/getting-x-and-y-coordinates-from-tile-id
	sf::IntRect sub_rect;
	sub_rect.top = m_tile_size.y * (tile_type / m_tile_map_columns);
	sub_rect.height = m_tile_size.y;
	sub_rect.left = m_tile_size.x * (tile_type % m_tile_map_columns);
	sub_rect.width = m_tile_size.x;

	//adjust rect height and width based on the tile (to correct the collider)
	switch (tile_type)
	{
	case kHorizontalImpactPlatformPart:
	case kHorizontalBluePlatformPart:
	case kHorizontalRedPlatformPart:
		sub_rect.height = 23;
		break;
	case kVerticalImpactPlatformPart:
	case kVerticalBluePlatformPart:
	case kVerticalRedPlatformPart:
		sub_rect.width = 20;
		sub_rect.left += 22;
		spawn_pos.x += 22;
		break;
	case kBluePlayer:
	case kRedPlayer:
		sub_rect.height = 61;
		sub_rect.left += 12;
		spawn_pos.x += 32;
		sub_rect.width = 40;
		break;
	case kSpikes:
		sub_rect.top += 34;
		sub_rect.left += 4;
		sub_rect.height = 30;
		sub_rect.width = 56;
		spawn_pos.y += 34;
		spawn_pos.x += 4;
		break;
	}

	return sub_rect;
}

//Written by Paul Bichler (D00242563)
TileFactory::TileFactory(TextureHolder& textures, const sf::Vector2u tile_size, SoundPlayer& sound_player)
	: m_textures(textures),
	  m_tile_size(tile_size),
	  m_soundPlayer(sound_player)
{
	m_tile_map_columns = textures.Get(Textures::kLevelTileSet).getSize().x / tile_size.x;
}

//Written by Paul Bichler (D00242563)
PlatformPart* TileFactory::CreatePlatformPart(const ETileType tile_type, sf::Vector2f spawn_pos, Platform* parent) const
{
	const sf::IntRect sub_rect = GetSubRect(tile_type, spawn_pos);

	PlatformPart* platform_part = new PlatformPart(m_textures, sub_rect, parent, tile_type);
	platform_part->setPosition(spawn_pos);
	parent->AddPlatformPart(platform_part);

	return platform_part;
}

//Written by Paul Bichler (D00242563)
Tile* TileFactory::CreateTile(const ETileType tile_type, sf::Vector2f spawn_pos, const bool has_collider) const
{
	const sf::IntRect sub_rect = GetSubRect(tile_type, spawn_pos);
	Tile* tile = new Tile(m_textures, sub_rect, tile_type);
	tile->SetActiveCollider(has_collider);
	tile->setPosition(spawn_pos);

	return tile;
}

//Written by Paul Bichler (D00242563)
Character* TileFactory::CreatePlayer(const ETileType tile_type, const ECharacterType type, sf::Vector2f spawn_pos) const
{
	Character* character = new Character(type, m_textures, GetSubRect(tile_type, spawn_pos), m_soundPlayer);
	character->setPosition(spawn_pos);
	return character;
}
