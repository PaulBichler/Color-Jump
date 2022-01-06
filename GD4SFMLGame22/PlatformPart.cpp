#include "PlatformPart.hpp"

PlatformPart::PlatformPart(const TextureHolder& textures, const sf::IntRect sub_rect, Platform* platform, const ETileType type)
	: Tile(textures, sub_rect, type),
	m_parent(platform)
{
}

Platform* PlatformPart::GetPlatform() const
{
	return m_parent;
}

sf::FloatRect PlatformPart::GetBoundingRect() const
{
	return GetWorldTransform().transformRect(m_sprite.getGlobalBounds());
}

unsigned PlatformPart::GetCategory() const
{
	return Category::kPlatform;
}
