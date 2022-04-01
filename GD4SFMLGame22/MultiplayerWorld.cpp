#include "MultiplayerWorld.hpp"
#include "CollisionHandler.hpp"
#include "Utility.hpp"

MultiplayerWorld::MultiplayerWorld(sf::RenderTarget& output_target, SoundPlayer& sounds)
	: World(output_target, sounds)
{
}

void MultiplayerWorld::Update(const sf::Time dt)
{
	World::Update(dt);

	if(m_client_player != nullptr)
	{
		sf::Vector2f camera_pos = m_camera.getCenter();
		camera_pos.x = m_client_player->getPosition().x;
		m_camera.setCenter(camera_pos);
	}
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

Character* MultiplayerWorld::AddCharacter(sf::Int32 identifier, bool is_client_player)
{
	Character* player_character = World::AddCharacter(identifier, is_client_player);

	if (is_client_player)
		m_client_player = player_character;

	return player_character;
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
		Platform* collided_platform = nullptr;
		if (CollisionHandler::HandlePlayerTileCollision(pair, collided_platform)) continue;

		//Check if a checkpoint has been reached by both players
		if (collided_platform != nullptr && collided_platform->GetPlatformType() == EPlatformType::kGoal)
		{
			Utility::Debug("Checkpoint reached!");
			//if (m_players[0]->IsOnPlatform(collided_platform) &&
			//	m_players[1]->IsOnPlatform(collided_platform))
			//{
			//	//Win
			//	m_win_callback();
			//}
		}

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
