#include "Platform.hpp"

#include <SFML/Graphics/Texture.hpp>

#include "PlatformPart.hpp"
#include "ResourceHolder.hpp"

Platform::Platform(EPlatformType platform_type, TextureHolder& textures)
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

bool Platform::DoesPlayerCollide(ECharacterType character_type)
{
	switch (m_type)
	{
	case EPlatformType::kImpact:
		if(character_type == ECharacterType::kBlue)
			SetType(EPlatformType::kBlue);
		else
			SetType(EPlatformType::kRed);
		break;
	case EPlatformType::kBlue:
		if(character_type != ECharacterType::kBlue)
			return false;
		break;
	case EPlatformType::kRed:
		if(character_type != ECharacterType::kRed)
			return false;
		break;
	}

	return true;
}

void Platform::SetType(EPlatformType type)
{
	if(m_type == type)
		return;

	m_type = type;

	for (const auto part : m_platform_parts)
	{
		switch (type)
		{
		case EPlatformType::kBlue:
		{
				sf::Texture& blue_texture = m_textures.Get(Textures::kImpactBluePlatform);
				part->SetSpriteTexture(blue_texture, sf::IntRect(0, 0, blue_texture.getSize().x, blue_texture.getSize().y));
		}
			break;
		case EPlatformType::kRed:
		{
				sf::Texture& red_texture = m_textures.Get(Textures::kImpactRedPlatform);
				part->SetSpriteTexture(red_texture, sf::IntRect(0, 0, red_texture.getSize().x, red_texture.getSize().y));	
		}
			break;
		}
	}
}
