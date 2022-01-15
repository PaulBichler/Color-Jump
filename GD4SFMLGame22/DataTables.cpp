#include "DataTables.hpp"
#include "ParticleType.hpp"

std::vector<ParticleData> InitializeParticleData()
{
	std::vector<ParticleData> data(static_cast<int>(ParticleType::kParticleCount));

	data[static_cast<int>(ParticleType::kPropellant)].m_color = sf::Color(255, 255, 50);
	data[static_cast<int>(ParticleType::kPropellant)].m_lifetime = sf::seconds(0.6f);

	data[static_cast<int>(ParticleType::kSmoke)].m_color = sf::Color(50, 50, 50);
	data[static_cast<int>(ParticleType::kSmoke)].m_lifetime = sf::seconds(4.f);

	return data;
}

CharacterData InitializeCharacterData()
{
	CharacterData data;

	data.m_JumpForce = -600.0f;
	data.m_gravityForce = 9.81f;
	data.m_movementSpeed = 200.0f;
	data.m_dragMultiplier = 0.5f;

	return data;
}



