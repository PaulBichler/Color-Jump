#pragma once
#include <functional>
#include <vector>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Time.hpp>

#include "ResourceIdentifiers.hpp"

class Aircraft;

struct Direction
{
	Direction(const float angle, const float distance)
		: m_angle(angle), m_distance(distance)
	{
	}
	float m_angle;
	float m_distance;
};

struct ParticleData
{
	sf::Color						m_color;
	sf::Time						m_lifetime;
};

std::vector<ParticleData> InitializeParticleData();