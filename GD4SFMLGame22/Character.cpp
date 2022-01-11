#include "Character.hpp"

#include <iostream>
#include <SFML/Graphics/RenderTarget.hpp>

#include "RayGround.hpp"
#include "ResourceHolder.hpp"
#include "TextNode.hpp"
#include "Utility.hpp"


Character::Character(ECharacterType type, const TextureHolder& textures, const sf::IntRect& texture_rect)
	: Entity(100),
	  m_type(type),
	  m_sprite(textures.Get(Textures::kLevelTileSet), texture_rect),
	  m_grounded(true)
{
	std::cout << "Character created." << std::endl;

	Utility::CentreOrigin(m_sprite);

	std::unique_ptr<RayGround> ray(new RayGround(this));
	m_ray = ray.get();
	AttachChild(std::move(ray));

	UpdateRay();
}

float Character::GetMaxSpeed()
{
	return 200;
}

unsigned Character::GetCategory() const
{
	if (m_type == ECharacterType::kRed)
	{
		return static_cast<int>(Category::kPlayerOne);
	}
	return static_cast<int>(Category::kPlayerTwo);
}

void Character::Jump()
{
	if (m_grounded == false)
	{
		return;
	}

	m_grounded = false;
	Accelerate(0, -600);
}

void Character::SetGrounded()
{
	m_grounded = true;
	SetVelocity(m_velocity.x, 0);
	setPosition(getPosition().x, getPosition().y - 2);
}

void Character::SetFalling()
{
	m_grounded = false;
}

ECharacterType Character::GetCharacterType() const
{
	return m_type;
}

void Character::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{
	Entity::UpdateCurrent(dt, commands);
	Accelerate(-m_velocity.x * 0.5f, 0);
	if (!m_grounded)
	{
		Accelerate(0, 9.81f);
	}

	UpdateRay();
}

void Character::UpdateRay() const
{
	m_ray->setPosition(0.f, 50.f);
}

void Character::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_sprite, states);
}

sf::FloatRect Character::GetBoundingRect() const
{
	return GetWorldTransform().transformRect(m_sprite.getGlobalBounds());
}
