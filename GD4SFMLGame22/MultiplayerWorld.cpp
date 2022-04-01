#include "MultiplayerWorld.hpp"
#include "CollisionHandler.hpp"
#include "PlatformPart.hpp"
#include "Utility.hpp"

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

void MultiplayerWorld::SetCamera()
{
}

void MultiplayerWorld::HandleCollisions()
{
	std::set<SceneNode::Pair> collision_pairs;
	m_sceneGraph.CheckSceneCollision(m_sceneGraph, collision_pairs, [this](SceneNode& node)
	{
		//check collisions only for players and RayGround objects
		return dynamic_cast<Character*>(&node) != nullptr || dynamic_cast<RayGround*>(&node) !=
			nullptr;
	});

	std::map<Character*, std::set<SceneNode::Pair>> player_pairs;

	for (auto players : m_players)
	{
		player_pairs.insert(std::pair<Character*, std::set<SceneNode::Pair> >(players, std::set<SceneNode::Pair>()));
	}

	for (const SceneNode::Pair& pair : collision_pairs)
	{
		if (CollisionHandler::PlatformCollision(pair, m_players, m_reached_goal_callback)) continue;

		//Get All Ground Ray Casts for player one and two
		CollisionHandler::GetGroundRayCasts(player_pairs, pair, Category::kRay);
	}

	CollisionHandler::PlayerGroundRayCast(player_pairs);
}

sf::FloatRect MultiplayerWorld::GetBattlefieldBounds() const
{
	return {};
}
