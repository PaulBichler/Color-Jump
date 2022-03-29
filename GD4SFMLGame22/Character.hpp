#pragma once
#include <SFML/Graphics/Sprite.hpp>

#include "Animation.hpp"
#include "Application.hpp"
#include "EColorType.hpp"
#include "Entity.hpp"
#include "Platform.hpp"
#include "RayGround.hpp"
#include "ResourceIdentifiers.hpp"

class Character : public Entity
{
private:
	EColorType m_type;
	sf::Sprite m_sprite;
	bool m_grounded;
	Platform* m_current_platform;
	RayGround* m_ray;
	bool m_can_jump;
	bool m_show_jump_animation{};
	Animation m_jump_smoke_animation;
	SoundPlayer& m_sounds;
	int m_identifier{};

public:
	Character(EColorType type, const TextureHolder& textures, const sf::IntRect& texture_rect, SoundPlayer& context);
	static float GetMaxSpeed();
	unsigned GetCategory() const override;
	void Jump();
	void SetGrounded(Platform* platform);
	void SetFalling();
	EColorType GetCharacterType() const;
	Platform* GetCurrentPlatform() const;
	bool IsOnPlatformOfType(EPlatformType platform_type) const;
	void StopMovement();
	void MoveOutOfCollision(const sf::FloatRect& rect);
	void SetIdentifier(int identifier);
	sf::Int32 GetIdentifier() const;
	void SetHitPoints(sf::Int32 hit_points);
	void SetGrounded();
protected:
	void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;
	void UpdateRay() const;
private:
	void DrawCurrent(sf::RenderTarget&, sf::RenderStates states) const override;
	sf::FloatRect GetBoundingRect() const override;
	
};
