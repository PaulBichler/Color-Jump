#pragma once
#include "Tile.hpp"
#include "ETileColor.hpp"

class ColorTile : public Tile
{
private:
	ETileColor m_color;

public:
	ColorTile(const TextureHolder& textures, sf::IntRect sub_rect, ETileType tile_type);
	ETileColor GetColor() const;
};

