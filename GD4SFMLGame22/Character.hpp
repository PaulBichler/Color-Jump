#pragma once
#include <SFML/Graphics/Sprite.hpp>

#include "ECharacterType.hpp"
#include "Entity.hpp"
#include "RayGround.hpp"
#include "ResourceIdentifiers.hpp"
#include "TextNode.hpp"

class Character : public Entity
{
private:
	ECharacterType m_type;
	sf::Sprite m_sprite;
	bool m_grounded;
	RayGround* m_ray;

public:
	Character(ECharacterType type, const TextureHolder& textures, const sf::IntRect& texture_rect);
	float GetMaxSpeed();
	unsigned GetCategory() const override;
	void Jump();
	void SetGrounded();
	void SetFalling();
protected:
	void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;
	void UpdateRay() const;
private:
	virtual void DrawCurrent(sf::RenderTarget&, sf::RenderStates states) const override;
	sf::FloatRect GetBoundingRect() const override;
};
