#include "CollisionHandler.hpp"
#include "SingleplayerWorld.hpp"

SinglePlayerWorld::SinglePlayerWorld(sf::RenderTarget& output_target, SoundPlayer& sounds, FontHolder& fonts)
	: World(output_target, sounds, fonts)
{
}

void SinglePlayerWorld::Update(sf::Time dt)
{
	DestroyEntitiesOutsideView();

	UpdateSounds();
	World::Update(dt);
}

void SinglePlayerWorld::Draw()
{
	World::Draw();
}

//Written by Paul Bichler (D00242563)
//Set the Lose Callback, which is invoked when a player dies.
//This is used to tell the Game State to push the Level Lose State.
void SinglePlayerWorld::SetLoseCallback(const std::function<void()>& callback)
{
	m_lose_callback = callback;
}

//Written by Paul Bichler (D00242563)
//Set the Win Callback, which is invoked when the level is completed.
//This is used to tell the Game State to push the Level Win State.
void SinglePlayerWorld::SetWinCallback(const std::function<void()>& callback)
{
	m_win_callback = callback;
}

//Written by Paul Bichler (D00242563)
//Returns camera bounds + a small area at the top to prevent players from dying when jumping out of the camera bounds.
sf::FloatRect SinglePlayerWorld::GetBattlefieldBounds() const
{
	sf::FloatRect bounds = GetViewBounds();
	bounds.top -= 300.f;
	bounds.height += 300.f;

	return bounds;
}

void SinglePlayerWorld::UpdateSounds() const
{
	if (!m_players.empty())
	{
		// Set listener's position to player position
		m_sounds.SetListenerPosition(m_players[0]->GetWorldPosition());

		// Remove unused sounds
		m_sounds.RemoveStoppedSounds();
	}
}

//Written by Paul Bichler (D00242563)
void SinglePlayerWorld::DestroyEntitiesOutsideView()
{
	Command command;
	command.category = Category::Type::kPlayer;
	command.action = DerivedAction<Entity>([this](Entity& e, sf::Time)
	{
		//Does the object intersect with the battlefield (Lose Condition)
		if (!GetBattlefieldBounds().intersects(e.GetBoundingRect()))
		{
			//Lose
			e.Destroy();
			m_lose_callback();
		}
	});
	m_command_queue.Push(command);
}



//Written by Dylan Goncalves Martins (), modified by Paul Bichler (D00242563)
void SinglePlayerWorld::HandleCollisions()
{
	std::set<SceneNode::Pair> collision_pairs;
	m_sceneGraph.CheckSceneCollision(m_sceneGraph, collision_pairs, [this](SceneNode& node)
	{
		//check collisions only for players and RayGround objects
		return dynamic_cast<Character*>(&node) != nullptr || dynamic_cast<RayGround*>(&node) !=
			nullptr;
	});

	std::set<SceneNode::Pair> pairs_player_one;
	std::set<SceneNode::Pair> pairs_player_two;

	for (const SceneNode::Pair& pair : collision_pairs)
	{
		if (CollisionHandler::PlatformCollision(pair, m_win_callback))
			continue;

		CollisionHandler::TrapCollision(pair, m_lose_callback);

		//Get All Ground Ray Casts for player one and two
		CollisionHandler::GetGroundRayCasts(pairs_player_one, pair, Category::kRayOne);
		CollisionHandler::GetGroundRayCasts(pairs_player_two, pair, Category::kRayTwo);
	}

	//Check Ground Ray Casts
	CollisionHandler::PlayerGroundRayCast(pairs_player_one);
	CollisionHandler::PlayerGroundRayCast(pairs_player_two);
}
