#pragma once
#include "Character.hpp"
#include "Utility.hpp"

class GhostCharacter : public Character
{
public:
	GhostCharacter(const EColorType type, const TextureHolder& textures, const FontHolder& fonts,
	               const sf::IntRect& texture_rect,
	               SoundPlayer& context)
		: Character(type, textures, fonts, texture_rect, context)
	{
	}

protected:
	void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;
private:
	sf::FloatRect GetBoundingRect() const override;
public:
	unsigned GetCategory() const override;
};
