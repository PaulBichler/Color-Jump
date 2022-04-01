#pragma once
#include "Character.hpp"

class GhostCharacter :public Character
{
public:
	GhostCharacter(const EColorType type, const TextureHolder& textures, const sf::IntRect& texture_rect,
		SoundPlayer& context)
		: Character(type, textures, texture_rect, context)
	{
	}

	unsigned GetCategory() const override;
};

