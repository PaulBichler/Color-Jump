#include "Entity.hpp"

#include "Utility.hpp"

Entity::Entity(const int hit_points)
	: m_hit_points(hit_points)
{
}

void Entity::SetVelocity(const sf::Vector2f velocity)
{
	m_velocity = velocity;
}

void Entity::SetVelocity(const float vx, const float vy)
{
	m_velocity.x = vx;
	m_velocity.y = vy;
}

sf::Vector2f Entity::GetVelocity() const
{
	return m_velocity;
}

void Entity::Accelerate(const sf::Vector2f velocity)
{
	m_velocity += velocity;
}

void Entity::Accelerate(const float vx, const float vy)
{
	m_velocity.x += vx;
	m_velocity.y += vy;
}

void Entity::UpdateCurrent(const sf::Time dt, CommandQueue& commands)
{
	move(m_velocity * dt.asSeconds());
}

int Entity::GetHitPoints() const
{
	return m_hit_points;
}

void Entity::Repair(const unsigned int points)
{
	m_hit_points += points;
}

void Entity::Damage(const unsigned int points)
{
	m_hit_points -= points;
}

void Entity::Destroy()
{
	m_hit_points = 0;
}

bool Entity::IsDestroyed() const
{
	return m_hit_points <= 0;
}
