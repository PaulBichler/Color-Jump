#include "SoundNode.hpp"
#include "SoundPlayer.hpp"


SoundNode::SoundNode(SoundPlayer& player)
	: m_sounds(player)
{
}

void SoundNode::PlaySound(const SoundEffect sound, const sf::Vector2f position) const
{
	m_sounds.Play(sound, position);
}

unsigned int SoundNode::GetCategory() const
{
	return Category::kSoundEffect;
}