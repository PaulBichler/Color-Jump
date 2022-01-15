#include "TileFactory.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "PlatformPart.hpp"
#include "ResourceHolder.hpp"
#include "Tile.hpp"

sf::IntRect TileFactory::GetSubRect(int pos, ETileType tile_type, sf::Vector2f& spawn_pos) const
{
	sf::IntRect sub_rect;
	/*subRect.top = m_tile_size.y * ((pos - 1) / m_tile_map_columns); */
	sub_rect.top = m_tile_size.y * (pos / m_tile_map_columns);
	sub_rect.height = m_tile_size.y;
	//subRect.left = m_tile_size.x * ((pos - 1) % m_tile_map_columns);
	sub_rect.left = m_tile_size.x * (pos % m_tile_map_columns);
	sub_rect.width = m_tile_size.x;

	//adjust rect height and width based on the tile (to correct the collider)
	switch (tile_type)
	{
	case kHorizontalImpactPlatform:
	case kBluePlatform:
	case kRedPlatform:
		sub_rect.height = 23;
		break;
	case kVerticalImpactPlatform:
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

TileFactory::TileFactory(TextureHolder& textures, const sf::Vector2u tile_size, SoundPlayer& sound_player)
	: m_textures(textures),
	  m_tile_size(tile_size),
	  m_soundPlayer(sound_player)
{
	m_tile_map_columns = textures.Get(Textures::kLevelTileSet).getSize().x / tile_size.x;
}

PlatformPart* TileFactory::CreatePlatformPart(const int pos, sf::Vector2f spawn_pos, Platform* parent,
                                              const ETileType tile_type) const
{
	const sf::IntRect sub_rect = GetSubRect(pos, tile_type, spawn_pos);

	PlatformPart* platform_part = new PlatformPart(m_textures, sub_rect, parent, tile_type);
	platform_part->setPosition(spawn_pos);
	parent->AddPlatformPart(platform_part);

	return platform_part;
}

Tile* TileFactory::CreateTile(const int pos, sf::Vector2f spawn_pos, const ETileType tile_type,
                              const bool has_collider) const
{
	const sf::IntRect sub_rect = GetSubRect(pos, tile_type, spawn_pos);
	Tile* tile = new Tile(m_textures, sub_rect, tile_type);
	tile->SetActiveCollider(has_collider);
	tile->setPosition(spawn_pos);

	return tile;
}

Character* TileFactory::CreatePlayer(const int id, const ETileType tile_type, const ECharacterType type,
                                     sf::Vector2f spawn_pos) const
{
	Character* character = new Character(type, m_textures, GetSubRect(id, tile_type, spawn_pos), m_soundPlayer);
	character->setPosition(spawn_pos);
	return character;
}
