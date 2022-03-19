#include "MultiplayerWorld.hpp"

#include "MultiplayerLevelLoader.hpp"

MultiplayerWorld::MultiplayerWorld(sf::RenderTarget& output_target, SoundPlayer& sounds, LevelManager& level_manager)
	: World(output_target, sounds, level_manager)
{
}

void MultiplayerWorld::Update(sf::Time dt)
{
	World::Update(dt);
}

void MultiplayerWorld::Draw()
{
	World::Draw();
}

Character* MultiplayerWorld::AddCharacter(const sf::Int32 identifier, const sf::Vector2f& position)
{
	const sf::IntRect rect(position.x, position.y, 15, 15);
	std::unique_ptr<Character> player(
		new Character(EColorType::kRed, m_textures, rect, m_sounds));
	player->setPosition(m_camera.getCenter());
	player->SetIdentifier(identifier);

	m_players.emplace_back(player.get());
	m_scene_layers[static_cast<int>(Layers::kUpperAir)]->AttachChild(std::move(player));
	return m_players.back();
}

Character* MultiplayerWorld::GetCharacter(const sf::Int32 identifier) const
{
	for (Character* character : m_players)
	{
		if (character->GetIdentifier() == identifier)
		{
			return character;
		}
	}
	return nullptr;
}

void MultiplayerWorld::RemoveCharacter(const sf::Int32 identifier)
{
	Character* character = GetCharacter(identifier);
	if (character)
	{
		character->Destroy();
		m_players.erase(std::find(m_players.begin(), m_players.end(), character));
	}
}

LevelInfo& MultiplayerWorld::BuildLevel(LevelManager::LevelData current_level_data)
{
	//This method is called in the constructor of the base World class
	MultiplayerLevelLoader level_loader(current_level_data, m_textures, m_sounds);
	m_level_info = level_loader.LoadLevel();

	return m_level_info;
}

void MultiplayerWorld::SetCamera()
{
}

void MultiplayerWorld::HandleCollisions()
{
}

sf::FloatRect MultiplayerWorld::GetBattlefieldBounds() const
{
	return sf::FloatRect();
}
