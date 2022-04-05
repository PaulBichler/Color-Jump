//Written by Dylan Goncalves Martins (D00242562), modified by Paul Bichler (D00242563)

#include "Character.hpp"

#include <iostream>
#include <SFML/Graphics/RenderTarget.hpp>

#include "DataTables.hpp"
#include "RayGround.hpp"
#include "ResourceHolder.hpp"
#include "TextNode.hpp"
#include "Utility.hpp"

namespace
{
	const CharacterData kTable = InitializeCharacterData();
}

void Character::CreateRay()
{
	std::unique_ptr<RayGround> ray(new RayGround(this));
	m_ray = ray.get();
	AttachChild(std::move(ray));
}

void Character::InitRay()
{
	CreateRay();
	UpdateRay();
}

void Character::SetColor()
{
	const auto& color = m_sprite.getColor();
	const auto& white = sf::Color(255, 255, 255, 50);
	m_sprite.setColor(sf::Color(color.r, color.g, color.b, 50));
	m_name_text.setFillColor(white);
	m_team_id_text.setFillColor(white);
}

void Character::ResetColor()
{
	const auto& color = m_sprite.getColor();
	const auto& white = sf::Color(255, 255, 255, 255);
	m_sprite.setColor(sf::Color(color.r, color.g, color.b, 255));
	m_name_text.setFillColor(white);
	m_team_id_text.setFillColor(white);
}

/*
 *	Dylan Goncalves Martins (D00242562)
 *	Creates the character for the players
 *	Also Creates a "Ray" and sets it as a child of the character
 */
Character::Character(const EColorType type, const TextureHolder& textures, const FontHolder& fonts,
                     const sf::IntRect& texture_rect,
                     SoundPlayer& context)
	: Entity(100),
	  m_type(type),
	  m_sprite(textures.Get(Textures::kLevelTileSet), texture_rect),
	  m_grounded(false),
	  m_current_platform(nullptr),
	  m_can_jump(true),
	  m_sounds(context),
	  m_jump_smoke_animation(textures.Get(Textures::kJumpSmoke))
{
	m_jump_smoke_animation.SetFrameSize(sf::Vector2i(256, 256));
	m_jump_smoke_animation.SetNumFrames(16);
	m_jump_smoke_animation.SetDuration(sf::seconds(.5f));
	m_jump_smoke_animation.setScale(.5f, .5f);

	const sf::FloatRect bounds = m_jump_smoke_animation.GetLocalBounds();
	m_jump_smoke_animation.setOrigin(std::floor(bounds.left + bounds.width / 2.f),
		std::floor(bounds.top + 50.f));

	m_name_text.setFont(fonts.Get(Fonts::Main));
	m_name_text.setCharacterSize(15.f);
	m_team_id_text.setFont(fonts.Get(Fonts::Main));
	m_team_id_text.setCharacterSize(15.f);

	Utility::Debug("Character created.");
	Utility::CentreOrigin(m_sprite);
}

float Character::GetMaxSpeed()
{
	return kTable.m_movementSpeed;
}

/*
 *	Dylan Goncalves Martins (D00242562)
 *	Used for controls
 *	PlayerOne -> red
 *	PlayerTwo -> blue
 */
unsigned Character::GetCategory() const
{
	if (m_type == EColorType::kRed)
	{
		return static_cast<int>(Category::kPlayerOne);
	}
	return static_cast<int>(Category::kPlayerTwo);
}

void Character::Debug()
{
	Utility::Debug("Id:       " + std::to_string(m_identifier));
	Utility::Debug("Team Id:  " + std::to_string(m_team_identifier));
	Utility::Debug("Color Id: " + std::to_string(static_cast<int>(m_type)));
	Utility::Debug("Name:     " + m_name);
}

/*
 *	Dylan Goncalves Martins (D00242562)
 *	Jump action for the player
 *	Check if player can jump is also done here
 */
void Character::Jump()
{
	Utility::Debug("Jump requested");
	Debug();

	if (m_can_jump == false)
	{
		return;
	}

	m_show_jump_animation = true;
	m_jump_smoke_animation.Restart();
	m_sounds.Play(SoundEffect::kJump);
	m_can_jump = false;
	m_grounded = false;
	m_current_platform = nullptr;
	SetVelocity(0, kTable.m_JumpForce);
}

/*
 *	Dylan Goncalves Martins (D00242562)
 *	This is called when the player lands on a platform
 *	Removes y axis velocity and moves character slightly up
 */
void Character::SetGrounded(Platform* platform)
{
	m_show_jump_animation = false;
	m_can_jump = true;
	m_grounded = true;
	m_current_platform = platform;
	SetVelocity(m_velocity.x, 0);
	setPosition(getPosition().x, getPosition().y - 2);
}

/*
 *	Dylan Goncalves Martins (D00242562)
 *	This is called when the Ray has no platform with which he collides
 */
void Character::SetFalling()
{
	m_grounded = false;
	m_current_platform = nullptr;
}

EColorType Character::GetCharacterType() const
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

bool Character::IsOnPlatform(Platform* platform) const
{
	return m_current_platform == platform;
}

/*
 *	Dylan Goncalves Martins (D00242562)
 *	Removes velocity on x axis and sets y axis to gravity force
 */
void Character::StopMovement()
{
	SetVelocity(0, kTable.m_gravityForce);
}

/*
 *	Dylan Goncalves Martins (D00242562)
 *	On Collision with platform from sides or underneath
 *	Moves the player back so he is not colliding anymore
 */
void Character::MoveOutOfCollision(const sf::FloatRect& rect)
{
	sf::Vector2f velocity = GetVelocity();

	if (velocity == sf::Vector2f(0, 0))
	{
		velocity = sf::Vector2f(0, -9.81f);
	}

	const sf::Vector2f normal_velocity = Utility::UnitVector(velocity);
	SetVelocity(0, 0);

	while (rect.intersects(GetBoundingRect()))
	{
		setPosition(getPosition() - normal_velocity);
	}
}

void Character::SetIdentifier(const sf::Int8 identifier)
{
	m_identifier = identifier;
}

void Character::SetTeamIdentifier(const sf::Int8 identifier)
{
	m_team_identifier = identifier;

	m_team_id_text.setString(std::to_string(identifier));
	const sf::FloatRect bounds = m_team_id_text.getLocalBounds();
	m_team_id_text.setOrigin(std::floor(bounds.left + bounds.width / 2.f),
	                         m_sprite.getOrigin().y - 5.0f);
}

void Character::SetName(const std::string& name)
{
	m_name = name;
	m_name_text.setString(name);
	const sf::FloatRect bounds = m_name_text.getLocalBounds();
	m_name_text.setOrigin(std::floor(bounds.left + bounds.width / 2.f),
	                      m_sprite.getOrigin().y + m_sprite.getTextureRect().height / 2.f - 5.f);
}

sf::Int8 Character::GetIdentifier() const
{
	return m_identifier;
}

sf::Int8 Character::GetTeamIdentifier() const
{
	return m_team_identifier;
}

std::string Character::GetName() const
{
	return m_name;
}

void Character::SetHitPoints(const sf::Int8 hit_points)
{
	m_hit_points = hit_points;
}

void Character::SetGrounded()
{
	m_show_jump_animation = false;
	m_can_jump = true;
	m_grounded = true;
	SetVelocity(m_velocity.x, 0);
	setPosition(getPosition().x, getPosition().y - 2);
}

sf::IntRect Character::GetSize() const
{
	return m_sprite.getTextureRect();
}

void Character::UpdateCurrent(const sf::Time dt, CommandQueue& commands)
{
	Entity::UpdateCurrent(dt, commands);
	Accelerate(-m_velocity.x * kTable.m_dragMultiplier, 0);
	if (!m_grounded)
	{
		Accelerate(0, kTable.m_gravityForce);
	}

	UpdateRay();

	if (m_show_jump_animation)
	{
		m_jump_smoke_animation.Update(dt);
		m_show_jump_animation = !m_jump_smoke_animation.IsFinished();
	}
}

/*
 *	Dylan Goncalves Martins (D00242562)
 *	Updates the ray position
 */
void Character::UpdateRay() const
{
	if (IsDestroyed())
		return;

	m_ray->setPosition(0.f, 50.f);
}

void Character::DrawCurrent(sf::RenderTarget& target, const sf::RenderStates states) const
{
	target.draw(m_sprite, states);
	target.draw(m_name_text, states);
	target.draw(m_team_id_text, states);

	if (m_show_jump_animation)
		target.draw(m_jump_smoke_animation, states);
}

sf::FloatRect Character::GetBoundingRect() const
{
	return GetWorldTransform().transformRect(m_sprite.getGlobalBounds());
}
