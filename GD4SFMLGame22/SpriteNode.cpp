#include "SpriteNode.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

SpriteNode::SpriteNode(const sf::Texture& texture):m_sprite(texture)
{
}

SpriteNode::SpriteNode(const sf::Texture& texture, const sf::IntRect& textureRect):m_sprite(texture, textureRect)
{
}

void SpriteNode::SetSpriteTexture(const sf::Texture& texture, const sf::IntRect& textureRect)
{
	m_sprite.setTextureRect(textureRect);
	m_sprite.setTexture(texture);
}

void SpriteNode::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_sprite, states);
}
