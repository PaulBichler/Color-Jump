#include "SingleplayerWorld.hpp"

#include "PlatformPart.hpp"

SingleplayerWorld::SingleplayerWorld(sf::RenderTarget& output_target, SoundPlayer& sounds, LevelManager& level_manager)
	: World(output_target, sounds, level_manager)
{
}

LevelInfo& SingleplayerWorld::BuildLevel(LevelManager::LevelData current_level_data)
{
	//This method is called in the constructor of the base World class
	SingleplayerLevelLoader level_loader(current_level_data, m_textures, m_sounds);
	m_level_info = level_loader.LoadLevel();

	return m_level_info;
}

void SingleplayerWorld::Update(sf::Time dt)
{
	DestroyEntitiesOutsideView();

	UpdateSounds();
	UpdatePlatforms(dt);
	World::Update(dt);
}

void SingleplayerWorld::Draw()
{
	World::Draw();
}

//Written by Paul Bichler (D00242563)
//Set the Lose Callback, which is invoked when a player dies.
//This is used to tell the Game State to push the Level Lose State.
void SingleplayerWorld::SetLoseCallback(const std::function<void()>& callback)
{
	m_lose_callback = callback;
}

//Written by Paul Bichler (D00242563)
//Set the Win Callback, which is invoked when the level is completed.
//This is used to tell the Game State to push the Level Win State.
void SingleplayerWorld::SetWinCallback(const std::function<void()>& callback)
{
	m_win_callback = callback;
}

void SingleplayerWorld::SetCamera()
{
}

//Written by Paul Bichler (D00242563)
//Returns camera bounds + a small area at the top to prevent players from dying when jumping out of the camera bounds.
sf::FloatRect SingleplayerWorld::GetBattlefieldBounds() const
{
	sf::FloatRect bounds = GetViewBounds();
	bounds.top -= 300.f;
	bounds.height += 300.f;

	return bounds;
}

void SingleplayerWorld::UpdateSounds() const
{
	// Set listener's position to player position
	m_sounds.SetListenerPosition(m_level_info.player_1->GetWorldPosition());

	// Remove unused sounds
	m_sounds.RemoveStoppedSounds();
}

//Written by Paul Bichler (D00242563)
//Updates the platforms (used by Pulse Platforms to change color every 2 seconds)
void SingleplayerWorld::UpdatePlatforms(const sf::Time dt) const
{
	for (const auto& platform : m_level_info.platforms)
		platform->Update(dt);
}

//Written by Paul Bichler (D00242563)
void SingleplayerWorld::DestroyEntitiesOutsideView()
{
	Command command;
	command.category = Category::Type::kPlayerOne | Category::Type::kPlayerTwo;
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
void SingleplayerWorld::HandleCollisions()
{
	std::set<SceneNode::Pair> collision_pairs;
	m_sceneGraph.CheckSceneCollision(m_sceneGraph, collision_pairs);

	std::set<SceneNode::Pair> pairs_player_one;
	std::set<SceneNode::Pair> pairs_player_two;

	for (SceneNode::Pair pair : collision_pairs)
	{
		if (MatchesCategories(pair, Category::Type::kPlayer, Category::Type::kPlatform))
		{
			auto& player = dynamic_cast<Character&>(*pair.first);
			auto& platform_part = dynamic_cast<PlatformPart&>(*pair.second);
			Platform* platform = platform_part.GetPlatform();

			//Checks if player collided from underneath the center of the platform
			if (IsPlayerBelowPlatform(player, platform_part))
			{
				//Checks if platform is collidable with player
				if (IsPlayerAtHisPlatform(player, platform))
				{
					// move player out of collision and stop his movement
					player.MoveOutOfCollision(platform_part.GetBoundingRect());
					player.StopMovement();

					// Set color of vertical platform if there is a collision from the side/underneath 
					if (platform->GetPlatformType() == EPlatformType::kVerticalImpact)
					{
						platform->DoesPlayerCollide(player.GetCharacterType());
					}
				}
				// continue to next pair
				continue;
			}

			if (platform->DoesPlayerCollide(player.GetCharacterType()))
			{
				//Collision
				player.SetGrounded(platform);
			}

			//Check Win Condition
			if (!m_has_won && platform->GetPlatformType() == EPlatformType::kGoal)
			{
				if (m_level_info.player_1->IsOnPlatformOfType(EPlatformType::kGoal) &&
					m_level_info.player_2->IsOnPlatformOfType(EPlatformType::kGoal))
				{
					//Win
					m_win_callback();
					m_has_won = true;
				}
			}
		}

		//Check Lose Condition
		if (MatchesCategories(pair, Category::Type::kPlayer, Category::Type::kEnemyTrap))
		{
			//Lose
			m_lose_callback();
		}

		//Get All Ground Ray Casts for player one and two
		GetGroundRayCasts(pairs_player_one, pair, Category::kRayOne);
		GetGroundRayCasts(pairs_player_two, pair, Category::kRayTwo);
	}

	//Check Ground Ray Casts
	PlayerGroundRayCast(pairs_player_one);
	PlayerGroundRayCast(pairs_player_two);
}

