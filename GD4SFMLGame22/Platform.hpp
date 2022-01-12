#pragma once
#include "ECharacterType.hpp"
#include "EPlatformType.hpp"
#include "Tile.hpp"

class PlatformPart;

class Platform
{
public:
	Platform(EPlatformType platform_type, TextureHolder& textures);
	EPlatformType GetPlatformType() const;
	void AddPlatformPart(PlatformPart* tile);
	virtual bool DoesPlayerCollide(ECharacterType character_type);
	void SetType(EPlatformType type);
protected:
	EPlatformType m_type;
	TextureHolder& m_textures;
	std::vector<PlatformPart*> m_platform_parts;
};

