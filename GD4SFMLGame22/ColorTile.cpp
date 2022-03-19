#include "ColorTile.hpp"
#include "ResourceHolder.hpp"

#include <SFML/Graphics/Texture.hpp>

ColorTile::ColorTile(TextureHolder& textures, const sf::IntRect sub_rect, const ETileType tile_type, const EColorType color_type)
	: Tile(textures, sub_rect, tile_type),
	  m_color(EColorType::kNone),
	  m_textures(textures),
	  m_is_vertical(tile_type == kVerticalImpactPlatformPart)
{
	SetColor(color_type);
}

void ColorTile::SetColor(EColorType color)
{
	if(color == m_color)
		return;

	sf::Texture& texture = m_textures.Get(m_is_vertical ? Textures::kVImpactBlankPlatform : Textures::kHImpactBlankPlatform);

	switch (color)
	{
	case EColorType::kBlue:
		texture = m_textures.Get(m_is_vertical ? Textures::kVImpactBluePlatform : Textures::kHImpactBluePlatform);
		break;
	case EColorType::kRed:
		texture = m_textures.Get(m_is_vertical ? Textures::kVImpactRedPlatform : Textures::kHImpactRedPlatform);
		break;
	case EColorType::kGreen:
		texture = m_textures.Get(m_is_vertical ? Textures::kVImpactGreenPlatform : Textures::kHImpactGreenPlatform);
		break;
	}

	ReplaceTexture(texture);
	m_color = color;
}

void ColorTile::ReplaceTexture(const sf::Texture& texture)
{
	SetSpriteTexture(texture, sf::IntRect(0, 0, texture.getSize().x, texture.getSize().y));
}

EColorType ColorTile::GetColor() const
{
	return m_color;
}

//Written by Paul Bichler (D00242563)
//This method is used to decide whether a player will collide with the platform.
//It also decides how the platform reacts to the collision.
bool ColorTile::HandleCollision(const EColorType color)
{
	SetColor(color);

	//switch (m_type)
	//{
	//	//Impact Platforms change color to the color of the colliding player
	//case EPlatformType::kHorizontalImpact:
	//	if (character_type == ECharacterType::kBlue)
	//		SetType(EPlatformType::kHorizontalBlue);
	//	else
	//		SetType(EPlatformType::kHorizontalRed);
	//	break;
	//	//Impact Platforms change color to the color of the colliding player
	//case EPlatformType::kVerticalImpact:
	//	if (character_type == ECharacterType::kBlue)
	//		SetType(EPlatformType::kVerticalBlue);
	//	else
	//		SetType(EPlatformType::kVerticalRed);
	//	break;
	//	//Only the Blue Player can collide with the blue platforms
	//case EPlatformType::kHorizontalBlue:
	//case EPlatformType::kVerticalBlue:
	//	if (character_type != ECharacterType::kBlue)
	//		return false;
	//	break;
	//	//Only the Red Player can collide with the red platforms
	//case EPlatformType::kHorizontalRed:
	//case EPlatformType::kVerticalRed:
	//	if (character_type != ECharacterType::kRed)
	//		return false;
	//	break;
	//}

	return true;
}

