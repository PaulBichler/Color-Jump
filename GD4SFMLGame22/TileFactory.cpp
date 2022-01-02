#include "TileFactory.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "PlatformPart.hpp"
#include "ResourceHolder.hpp"
#include "SpriteNode.hpp"
#include "Tile.hpp"

sf::IntRect TileFactory::GetSubRect(int pos) const
{
	sf::IntRect subRect;
	/*subRect.top = m_tile_size.y * ((pos - 1) / m_tile_map_columns); */
	subRect.top = m_tile_size.y * (pos / m_tile_map_columns); 
	subRect.height = m_tile_size.y;
	//subRect.left = m_tile_size.x * ((pos - 1) % m_tile_map_columns);
	subRect.left = m_tile_size.x * (pos % m_tile_map_columns);
	subRect.width = m_tile_size.x;

	return subRect;
}

TileFactory::TileFactory(TextureHolder& textures, sf::Vector2u tile_size)
	: m_textures(textures),
	m_tile_size(tile_size)
{
	m_tile_map_columns = textures.Get(Textures::kLevelTileSet).getSize().x / tile_size.x;
}

PlatformPart* TileFactory::CreatePlatformPart(int pos, sf::Vector2f spawn_pos, Platform* parent, ETileType tile_type) const
{
	const sf::IntRect sub_rect = GetSubRect(pos);
	PlatformPart* platform_part = new PlatformPart(m_textures, sub_rect, parent, tile_type);
	platform_part->setPosition(spawn_pos);
	parent->AddPlatformPart(platform_part);

	return platform_part;
}

Tile* TileFactory::CreateTile(int pos, sf::Vector2f spawn_pos, ETileType tile_type) const
{
	const sf::IntRect sub_rect = GetSubRect(pos);

	//switch (static_cast<ETileType>(pos))
	//{
	///*case kPlatform:
	//	return CreatePlatformPart(sub_rect);*/
	////case kBluePlatform:
	////	return CreateBluePlatform();
	////case kRedPlatform:
	////	return CreateRedPlatform();
	////case kImpactPlatform:
	////	return CreateImpactPlatform();
	////case kFinish:
	////	return CreateFinishTile();
	//default:
	//	assert(false && "Tile Type could not be found in enumeration!");
	//	break;
	//}

	Tile* tile = new Tile(m_textures, sub_rect, tile_type);
	tile->setPosition(spawn_pos);

	return tile;
}

Character* TileFactory::CreatePlayer(int id, ECharacterType type, sf::Vector2f spawn_pos) const
{
	Character* character = new Character(type, m_textures, GetSubRect(id));
	character->setPosition(spawn_pos);
	return character;
}
