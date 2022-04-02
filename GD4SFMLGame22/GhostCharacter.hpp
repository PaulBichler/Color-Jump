#pragma once
#include "Character.hpp"

class GhostCharacter :public Character
{
public:
	GhostCharacter(const EColorType type, const TextureHolder& textures, const FontHolder& fonts, const sf::IntRect& texture_rect,
		SoundPlayer& context)
		: Character(type, textures, fonts, texture_rect, context)
	{
	}

	unsigned GetCategory() const override;
};

