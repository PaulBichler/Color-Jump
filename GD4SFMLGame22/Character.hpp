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
protected:
	EColorType m_type;
	std::string m_name;
	sf::Sprite m_sprite;
	bool m_grounded;
	Platform* m_current_platform;
	RayGround* m_ray{};
	bool m_can_jump;
	
	sf::Text m_name_text;
	SoundPlayer& m_sounds;
	int m_identifier{};
	int m_team_identifier{};

public:
	
	Character(EColorType type, const TextureHolder& textures, const FontHolder& fonts, const sf::IntRect& texture_rect, SoundPlayer& context);
	static float GetMaxSpeed();
	unsigned GetCategory() const override;
	void Debug();
	void Jump();
	void SetGrounded(Platform* platform);
	void SetFalling();
	EColorType GetCharacterType() const;
	Platform* GetCurrentPlatform() const;
	bool IsOnPlatformOfType(EPlatformType platform_type) const;
	bool IsOnPlatform(Platform* platform) const;
	void StopMovement();
	void MoveOutOfCollision(const sf::FloatRect& rect);
	void SetIdentifier(int identifier);
	void SetTeamIdentifier(int identifier);
	void SetName(const std::string& name);
	sf::Int32 GetIdentifier() const;
	sf::Int32 GetTeamIdentifier() const;
	std::string GetName() const;
	void SetHitPoints(sf::Int32 hit_points);
	void SetGrounded();

protected:
	void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;
	void UpdateRay() const;
	void CreateRay();

	bool m_show_jump_animation{};
	Animation m_jump_smoke_animation;
private:
	void DrawCurrent(sf::RenderTarget&, sf::RenderStates states) const override;
	sf::FloatRect GetBoundingRect() const override;
	
};
