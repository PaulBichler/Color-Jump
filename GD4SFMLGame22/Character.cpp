#include "Character.hpp"

#include <iostream>
#include <SFML/Graphics/RenderTarget.hpp>

#include "ResourceHolder.hpp"
#include "TextNode.hpp"

Character::Character(ECharacterType type, const TextureHolder& textures, const sf::IntRect& texture_rect)
	: Entity(100),
	m_type(type),
	m_sprite(textures.Get(Textures::kLevelTileSet), texture_rect)
{
	std::cout << "Character created." << std::endl;
}

float Character::GetMaxSpeed()
{
	return 200;
}

unsigned Character::GetCategory() const
{
	if (m_type == ECharacterType::kRed)
	{
		return static_cast<int>(Category::kPlayerOne);
	}
	else
	{
		return static_cast<int>(Category::kPlayerTwo);
	}
}

void Character::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_sprite, states);
}

sf::FloatRect Character::GetBoundingRect() const
{
	return GetWorldTransform().transformRect(m_sprite.getGlobalBounds());
}
