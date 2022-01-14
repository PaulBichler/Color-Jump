#include "Platform.hpp"

#include <SFML/Graphics/Texture.hpp>

#include "PlatformPart.hpp"
#include "ResourceHolder.hpp"
#include "Utility.hpp"

Platform::Platform(const EPlatformType platform_type, TextureHolder& textures)
	: m_type(platform_type),
	  m_textures(textures),
	  m_current_texture(nullptr),
	  m_current_pulse_cooldown(2.f)
{
	SetType(platform_type);
}

EPlatformType Platform::GetPlatformType() const
{
	return m_type;
}

void Platform::AddPlatformPart(PlatformPart* tile)
{
	m_platform_parts.emplace_back(tile);

	if(m_is_pulse)
		tile->SetSpriteTexture(*m_current_texture, sf::IntRect(0, 0, (*m_current_texture).getSize().x, (*m_current_texture).getSize().y));
}

bool Platform::DoesPlayerCollide(const ECharacterType character_type)
{
	switch (m_type)
	{
	case EPlatformType::kHorizontalImpact:
		if (character_type == ECharacterType::kBlue)
			SetType(EPlatformType::kHorizontalBlue);
		else
			SetType(EPlatformType::kHorizontalRed);
		break;
	case EPlatformType::kVerticalImpact:
		if (character_type == ECharacterType::kBlue)
			SetType(EPlatformType::kVerticalBlue);
		else
			SetType(EPlatformType::kVerticalRed);
		break;
	case EPlatformType::kHorizontalBlue:
	case EPlatformType::kVerticalBlue:
		if (character_type != ECharacterType::kBlue)
			return false;
		break;
	case EPlatformType::kHorizontalRed:
	case EPlatformType::kVerticalRed:
		if (character_type != ECharacterType::kRed)
			return false;
		break;
	case EPlatformType::kNormal:
	case EPlatformType::kGoal:;
	}

	return true;
}

void Platform::SetType(const EPlatformType type)
{
	Utility::Debug("Type set.");

	m_type = type;


	switch (type)
	{
	case EPlatformType::kHorizontalBlue:
		{
			SetTextureOnParts(m_textures.Get(Textures::kHImpactBluePlatform));
		}
		break;
	case EPlatformType::kHorizontalRed:
		{
			SetTextureOnParts(m_textures.Get(Textures::kHImpactRedPlatform));
		}
		break;
	case EPlatformType::kVerticalBlue:
		{
			SetTextureOnParts(m_textures.Get(Textures::kVImpactBluePlatform));
		}
		break;
	case EPlatformType::kVerticalRed:
		{
			SetTextureOnParts(m_textures.Get(Textures::kVImpactRedPlatform));
		}
		break;
	case EPlatformType::kHorizontalPulse:
		{
			m_is_pulse = true;
			SetType(EPlatformType::kHorizontalRed);
		}
		break;
	case EPlatformType::kNormal:
	case EPlatformType::kGoal:;
	}
}

void Platform::Update(sf::Time dt)
{
	if(!m_is_pulse)
		return;

	m_current_pulse_cooldown -= dt.asSeconds();

	if(m_current_pulse_cooldown <= 0)
	{
		switch (m_type)
		{
		case EPlatformType::kHorizontalBlue:
			SetType(EPlatformType::kHorizontalRed);
			break;
		case EPlatformType::kHorizontalRed:
			SetType(EPlatformType::kHorizontalBlue);
			break;
		case EPlatformType::kVerticalBlue:
			SetType(EPlatformType::kVerticalRed);
			break;
		case EPlatformType::kVerticalRed:
			SetType(EPlatformType::kVerticalBlue);
			break;
		}

		m_current_pulse_cooldown = 2.f;
	}
}

void Platform::SetTextureOnParts(sf::Texture& texture)
{
	m_current_texture = &texture;

	for (const auto part : m_platform_parts)
		part->SetSpriteTexture(texture, sf::IntRect(0, 0, texture.getSize().x, texture.getSize().y));
}
