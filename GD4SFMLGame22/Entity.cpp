#include "Entity.hpp"

Entity::Entity(const int hitPoints)
	: m_hitPoints(hitPoints)
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
	return m_hitPoints;
}

void Entity::Repair(const unsigned int points)
{
	assert(points > 0);
	m_hitPoints += points;
}

void Entity::Damage(const unsigned int points)
{
	assert(points > 0);
	m_hitPoints -= points;
}

void Entity::Destroy()
{
	m_hitPoints = 0;
}

bool Entity::IsDestroyed() const
{
	return m_hitPoints <= 0;
}
