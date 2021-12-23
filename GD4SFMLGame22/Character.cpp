#include "Character.hpp"

#include "ResourceHolder.hpp"

Character::Character(ECharacterType type, const TextureHolder& textures, const sf::IntRect& texture_rect)
	: Entity(0),
	m_type(type),
	m_sprite(textures.Get(Textures::kLevelTileSet), texture_rect)
{

}
