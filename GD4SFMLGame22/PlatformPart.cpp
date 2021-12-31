#include "PlatformPart.hpp"

PlatformPart::PlatformPart(const TextureHolder& textures, const sf::IntRect sub_rect, Platform* platform)
	: Tile(textures, sub_rect),
	m_parent(platform)
{
}
