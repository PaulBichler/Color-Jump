#include "RayGround.hpp"

#include "Character.hpp"

RayGround::RayGround(Character* character) : m_character(character)
{
}

unsigned RayGround::GetCategory() const
{
	if (m_character->GetCategory() == Category::kPlayerOne)
	{
		return Category::kRayOne;
	}

	return Category::kRayTwo;
}

sf::FloatRect RayGround::GetBoundingRect() const
{
	return {m_character->getPosition().x, m_character->getPosition().y, 2, 50};
}

void RayGround::SetFalling() const
{
	m_character->SetFalling();
}
