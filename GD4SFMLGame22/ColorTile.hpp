#pragma once
#include "EColorType.hpp"
#include "Tile.hpp"

class ColorTile : public Tile
{
private:
	EColorType m_color;
	TextureHolder& m_textures;

	void SetColor(EColorType color);
	void ReplaceTexture(const sf::Texture& texture);

public:
	ColorTile(TextureHolder& textures, sf::IntRect sub_rect, ETileType tile_type, EColorType color_type);
	bool HandleCollision(EColorType color) override;
	EColorType GetColor() const;
	unsigned GetCategory() const override;

private:
	bool m_is_vertical;
};

