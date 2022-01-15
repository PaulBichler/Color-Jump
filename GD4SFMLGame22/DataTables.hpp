#pragma once
#include <vector>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Time.hpp>

#include "ResourceIdentifiers.hpp"



struct ParticleData
{
	sf::Color	m_color;
	sf::Time	m_lifetime;
};

struct CharacterData
{
	float	m_movementSpeed;
	float	m_JumpForce;
	float	m_gravityForce;
	float	m_dragMultiplier;
};

std::vector<ParticleData> InitializeParticleData();
CharacterData InitializeCharacterData();