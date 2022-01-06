#pragma once
#include "CommandQueue.hpp"
#include "SceneNode.hpp"

class Entity : public SceneNode
{
public:
	explicit Entity(int hitPoints);
	virtual void SetVelocity(sf::Vector2f velocity);
	virtual void SetVelocity(float vx, float vy);
	virtual void Accelerate(sf::Vector2f velocity);
	virtual void Accelerate(float vx, float vy);
	sf::Vector2f GetVelocity() const;

	int GetHitPoints() const;
	void Repair(unsigned int points);
	void Damage(unsigned int points);
	void Destroy();
	virtual bool IsDestroyed() const override;

protected:
	void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;
	sf::Vector2f m_velocity;

private:
	unsigned int m_hitPoints;
};
