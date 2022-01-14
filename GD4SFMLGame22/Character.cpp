#include "Character.hpp"

#include <iostream>
#include <SFML/Graphics/RenderTarget.hpp>

#include "RayGround.hpp"
#include "ResourceHolder.hpp"
#include "TextNode.hpp"
#include "Utility.hpp"


Character::Character(const ECharacterType type, const TextureHolder& textures, const sf::IntRect& texture_rect)
	: Entity(100),
	  m_type(type),
	  m_sprite(textures.Get(Textures::kLevelTileSet), texture_rect),
	  m_grounded(true),
	  m_current_platform(nullptr)
{
	Utility::Debug("Character created.");
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
	if (m_canJump == false)
	{
		return;
	}

	m_canJump = false;
	m_grounded = false;
	m_current_platform = nullptr;
	SetVelocity(0, -600);
}

void Character::SetGrounded(Platform* platform)
{
	m_canJump = true;
	m_grounded = true;
	m_current_platform = platform;
	SetVelocity(m_velocity.x, 0);
	setPosition(getPosition().x, getPosition().y - 2);
}

void Character::SetFalling()
{
	Utility::Debug("Character set falling.");
	m_grounded = false;
	m_current_platform = nullptr;
}

ECharacterType Character::GetCharacterType() const
{
	return m_type;
}

Platform* Character::GetCurrentPlatform() const
{
	return m_current_platform;
}

bool Character::IsOnPlatformOfType(const EPlatformType platform_type) const
{
	if (m_current_platform == nullptr)
		return false;

	return m_current_platform->GetPlatformType() == platform_type;
}

void Character::StopMovement()
{
	SetVelocity(0, 9.81f);
}

void Character::MoveOutOfCollision(const sf::FloatRect& rect)
{
	const sf::Vector2f velocity = GetVelocity();
	const sf::Vector2f normalVelocity = Utility::UnitVector(velocity);
	SetVelocity(0, 0);

	while (rect.intersects(GetBoundingRect()))
	{
		setPosition(getPosition() - normalVelocity);
	}
}

void Character::UpdateCurrent(const sf::Time dt, CommandQueue& commands)
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
	if (IsDestroyed())
		return;

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
