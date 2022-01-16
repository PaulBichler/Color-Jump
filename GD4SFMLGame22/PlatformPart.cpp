//Written by Paul Bichler (D00242563)

#include "PlatformPart.hpp"

//Written by Paul Bichler (D00242563)
PlatformPart::PlatformPart(const TextureHolder& textures, const sf::IntRect sub_rect, Platform* platform, const ETileType type)
	: Tile(textures, sub_rect, type),
	m_parent(platform)
{
}

//Written by Paul Bichler (D00242563)
//When a player collides with a platform part, this method is used to get the platform it belongs to,
//which is then used to handle the collision
Platform* PlatformPart::GetPlatform() const
{
	return m_parent;
}

//Written by Paul Bichler (D00242563)
sf::FloatRect PlatformPart::GetBoundingRect() const
{
	return GetWorldTransform().transformRect(m_sprite.getGlobalBounds());
}

//Written by Paul Bichler (D00242563)
unsigned PlatformPart::GetCategory() const
{
	return Category::kPlatform;
}
