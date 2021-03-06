#pragma once
#include "EColorType.hpp"
#include "EPlatformType.hpp"
#include "Tile.hpp"

class PlatformPart;

class Platform
{
public:
	Platform(sf::Int8 id, EPlatformType platform_type, TextureHolder& textures);
	EPlatformType GetPlatformType() const;
	void AddPlatformPart(PlatformPart* tile);
	bool HandlePlayerCollisionAndChangeColor(EColorType color_type);
	virtual bool HandlePlayerCollision(EColorType color_type);
	void SetType(EPlatformType type);
	void ResetToInitialType();

	void Update(sf::Time dt);
	sf::Int8 GetID() const;
	std::vector<PlatformPart*>& GetParts();

protected:
	EPlatformType m_type;
	EPlatformType m_initial_type;
	TextureHolder& m_textures;
	std::vector<PlatformPart*> m_platform_parts;
	sf::Texture* m_current_texture;
	bool m_is_pulse{};
	float m_current_pulse_cooldown;
	sf::Int8 m_id;

private:
	void SetTextureOnParts(sf::Texture& texture);
};

