#include "Platform.hpp"

Platform::Platform()
	: m_type(EPlatformType::kNormal)
{
}

void Platform::AddPlatformPart(PlatformPart* tile)
{
	m_platform_parts.emplace_back(tile);
}
