#pragma once
#include "SceneNode.hpp"
#include <SFML/Graphics/Sprite.hpp>

class SpriteNode : public SceneNode
{
public:
	explicit SpriteNode(const sf::Texture& texture);
	SpriteNode(const sf::Texture& texture, const sf::IntRect& textureRect);

	void SetSpriteTexture(const sf::Texture& texture, const sf::IntRect& textureRect);

private:
	void DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;

protected:
	sf::Sprite m_sprite;
};

