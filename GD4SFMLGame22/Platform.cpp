#include "Platform.hpp"

#include <SFML/Graphics/Texture.hpp>

#include "PlatformPart.hpp"
#include "ResourceHolder.hpp"
#include "Utility.hpp"

Platform::Platform(const EPlatformType platform_type, TextureHolder& textures)
	: m_type(platform_type),
	  m_textures(textures)
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

	for (const auto part : m_platform_parts)
	{
		switch (type)
		{
		case EPlatformType::kHorizontalBlue:
			{
				sf::Texture& texture = m_textures.Get(Textures::kHImpactBluePlatform);
				part->SetSpriteTexture(texture, sf::IntRect(0, 0, texture.getSize().x, texture.getSize().y));
			}
			break;
		case EPlatformType::kHorizontalRed:
			{
				sf::Texture& texture = m_textures.Get(Textures::kHImpactRedPlatform);
				part->SetSpriteTexture(texture, sf::IntRect(0, 0, texture.getSize().x, texture.getSize().y));
			}
			break;
		case EPlatformType::kVerticalBlue:
			{
				sf::Texture& texture = m_textures.Get(Textures::kVImpactBluePlatform);
				part->SetSpriteTexture(texture, sf::IntRect(0, 0, texture.getSize().x, texture.getSize().y));
			}
			break;
		case EPlatformType::kVerticalRed:
			{
				sf::Texture& texture = m_textures.Get(Textures::kVImpactRedPlatform);
				part->SetSpriteTexture(texture, sf::IntRect(0, 0, texture.getSize().x, texture.getSize().y));
			}
			break;
		case EPlatformType::kNormal:
		case EPlatformType::kGoal:;
		}
	}
}
