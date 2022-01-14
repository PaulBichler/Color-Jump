#pragma once
#include "ETileType.hpp"
#include "ResourceIdentifiers.hpp"
#include "SpriteNode.hpp"

class Tile : public SpriteNode
{
public:
	explicit Tile(const TextureHolder& textures, const sf::IntRect sub_rect, const ETileType type);

	ETileType GetType() const;
	void SetActiveCollider(bool active);
	unsigned GetCategory() const override;
	sf::FloatRect GetBoundingRect() const override;

protected:
	ETileType m_type;
	bool m_has_collider;
};

