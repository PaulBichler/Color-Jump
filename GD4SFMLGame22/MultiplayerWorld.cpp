#include "MultiplayerWorld.hpp"

#include "CollisionHandler.hpp"
#include "MultiplayerLevelLoader.hpp"
#include "PlatformPart.hpp"

MultiplayerWorld::MultiplayerWorld(sf::RenderTarget& output_target, SoundPlayer& sounds)
	: World(output_target, sounds)
{
}

void MultiplayerWorld::Update(const sf::Time dt)
{
	World::Update(dt);
}

void MultiplayerWorld::Draw()
{
	World::Draw();
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
	std::cout << "Load Level" << std::endl;
	//This method is called in the constructor of the base World class
	MultiplayerLevelLoader level_loader(current_level_data, m_textures, m_sounds);
	m_level_info = static_cast<LevelInfo>(level_loader.LoadLevel());

	return m_level_info;
}

void MultiplayerWorld::SetCamera()
{
}

void MultiplayerWorld::HandleCollisions()
{
	std::set<SceneNode::Pair> collision_pairs;
	m_sceneGraph.CheckSceneCollision(m_sceneGraph, collision_pairs, [this](SceneNode& node)
	{
		//check collisions only for players and RayGround objects
		return dynamic_cast<Character*>(&node) != nullptr || dynamic_cast<RayGround*>(&node) != nullptr;
	});

	std::set<SceneNode::Pair> pairs_player_one;
	std::set<SceneNode::Pair> pairs_player_two;

	for (const SceneNode::Pair& pair : collision_pairs)
	{
		if (CollisionHandler::HandlePlayerTileCollision(pair)) continue;

		//Get All Ground Ray Casts for player one and two
		GetGroundRayCasts(pairs_player_one, pair, Category::kRayOne);
		GetGroundRayCasts(pairs_player_two, pair, Category::kRayTwo);
	}

	//Check Ground Ray Casts
	PlayerGroundRayCast(pairs_player_one);
	PlayerGroundRayCast(pairs_player_two);
}

sf::FloatRect MultiplayerWorld::GetBattlefieldBounds() const
{
	return {};
}
