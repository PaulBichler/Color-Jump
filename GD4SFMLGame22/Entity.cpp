#include "Entity.hpp"

Entity::Entity(const int hit_points)
	: m_hitpoints(hit_points)
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
	SetVelocity(m_velocity * 0.5f);
}

int Entity::GetHitPoints() const
{
	return m_hitpoints;
}

void Entity::Repair(const unsigned int points)
{
	assert(points > 0);
	m_hitpoints += points;
}

void Entity::Damage(const unsigned int points)
{
	assert(points > 0);
	m_hitpoints -= points;
}

void Entity::Destroy()
{
	m_hitpoints = 0;
}

bool Entity::IsDestroyed() const
{
	return m_hitpoints <= 0;
}
