#pragma once
#include "Platform.hpp"
#include "Tile.hpp"

class PlatformPart : public Tile
{
public:
	PlatformPart(const TextureHolder& textures, const sf::IntRect sub_rect, Platform* platform);
	Platform* GetPlatform() const;

private:
	Platform* m_parent;
};

