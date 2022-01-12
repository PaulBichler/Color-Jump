#pragma once
#include <SFML/Graphics/Sprite.hpp>

#include "ECharacterType.hpp"
#include "Entity.hpp"
#include "Platform.hpp"
#include "RayGround.hpp"
#include "ResourceIdentifiers.hpp"
#include "TextNode.hpp"

class Character : public Entity
{
private:
	ECharacterType m_type;
	sf::Sprite m_sprite;
	bool m_grounded;
	Platform* m_current_platform;
	RayGround* m_ray;

public:
	Character(ECharacterType type, const TextureHolder& textures, const sf::IntRect& texture_rect);
	float GetMaxSpeed();
	unsigned GetCategory() const override;
	void Jump();
	void SetGrounded(Platform* platform);
	void SetFalling();
	ECharacterType GetCharacterType() const;
	Platform* GetCurrentPlatform() const;
	bool IsOnPlatformOfType(EPlatformType platform_type) const;
protected:
	void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;
	void UpdateRay() const;
private:
	virtual void DrawCurrent(sf::RenderTarget&, sf::RenderStates states) const override;
	sf::FloatRect GetBoundingRect() const override;
};
