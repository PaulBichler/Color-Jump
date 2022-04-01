#pragma once
#include "Platform.hpp"
#include "Tile.hpp"

class PlatformPart : public Tile
{
public:
	PlatformPart(const TextureHolder& textures, sf::IntRect sub_rect, Platform* platform, ETileType type);
	Platform* GetPlatform() const;
	sf::FloatRect GetBoundingRect() const override;
	unsigned GetCategory() const override;

private:
	Platform* m_parent;
};
