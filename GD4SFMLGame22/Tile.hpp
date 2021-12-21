#pragma once
#include "ResourceIdentifiers.hpp"
#include "SpriteNode.hpp"

class Tile : public SpriteNode
{
public:
	explicit Tile(const TextureHolder& textures, const sf::IntRect sub_rect);
protected:

};

