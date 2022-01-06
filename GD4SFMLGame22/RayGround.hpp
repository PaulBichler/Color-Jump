#pragma once
#include "SceneNode.hpp"

class Character;

class RayGround : public SceneNode
{
public:
	explicit RayGround(Character* character);
	unsigned GetCategory() const override;
	sf::FloatRect GetBoundingRect() const override;
	void SetFalling() const;
	Character* m_character;
};
