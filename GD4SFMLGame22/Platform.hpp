#pragma once
#include "EPlatformType.hpp"
#include "Tile.hpp"

class PlatformPart;

class Platform
{
public:
	Platform();
	void AddPlatformPart(PlatformPart* tile);

private:
	EPlatformType m_type;
	std::vector<PlatformPart*> m_platform_parts;
};

