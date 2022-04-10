#include "CollisionHandler.hpp"

#include "Character.hpp"
#include "MultiplayerGameState.hpp"
#include "PlatformPart.hpp"
#include "Utility.hpp"

bool CollisionHandler::CheckPlatform(const Platform* platform, const EColorType character)
{
	const auto platform_type = platform->GetPlatformType();

	if (character == EColorType::kBlue)
	{
		if (platform_type == EPlatformType::kHorizontalBlue ||
			platform_type == EPlatformType::kVerticalBlue)
		{
			return true;
		}
	}
	else if (character == EColorType::kRed)
	{
		if (platform_type == EPlatformType::kHorizontalRed ||
			platform_type == EPlatformType::kVerticalRed)
		{
			return true;
		}
	}

	return false;
}

bool CollisionHandler::IsVerticalPlatform(const EPlatformType platform_type)
{
	return platform_type == EPlatformType::kVerticalBlue ||
		platform_type == EPlatformType::kVerticalImpact ||
		platform_type == EPlatformType::kVerticalRed;
}

/**
 * Dylan Goncalves Martins (D00242562)
 * \brief if the client collides with a platform the world informs the server of the collision
 * \param player client
 * \param platform  platform
 * \param multiplayer_world world
 */
void CollisionHandler::GroundPlayerAndChangePlatformColor(Character& player, Platform* platform,
                                                          MultiplayerWorld* multiplayer_world)
{
	//Ground players
	if (platform->HandlePlayerCollisionAndChangeColor(player.GetCharacterType()))
	{
		const sf::Int8 player_id = player.GetIdentifier();
		const sf::Int8 platform_id = platform->GetID();
		const auto platform_type = platform->GetPlatformType();

		if (multiplayer_world != nullptr)
			multiplayer_world->m_state->SendPlatformInfo(player_id, platform_id, platform_type);

		if (IsVerticalPlatform(platform_type))
			return;

		player.SetGrounded(platform);
	}
}


bool CollisionHandler::MatchesCategories(SceneNode::Pair& collision, Category::Type type1,
                                         Category::Type type2)
{
	const unsigned int category1 = collision.first->GetCategory();
	const unsigned int category2 = collision.second->GetCategory();

	if (type1 & category1 && type2 & category2)
	{
		return true;
	}

	if (type1 & category2 && type2 & category1)
	{
		std::swap(collision.first, collision.second);
		return true;
	}

	return false;
}

void CollisionHandler::IsAtTheFinishLine(const Character* player_1, const Character* player_2,
                                         const std::function<void()>& checkpoint_callback,
                                         const std::function<void()>& win_callback,
                                         const Platform* platform)
{
	const EPlatformType platform_type = platform->GetPlatformType();

	//Check Win Condition
	if (platform_type == EPlatformType::kCheckpoint)
	{
		if (player_1->IsOnPlatformOfType(platform_type))
		{
			checkpoint_callback();
		}
	}

	if (platform_type == EPlatformType::kGoal)
	{
		if (player_1 == nullptr ||
			player_2 == nullptr)
			return;

		const bool player_1_check = player_1->IsOnPlatformOfType(platform_type);
		const bool player_2_check = player_2->IsOnPlatformOfType(platform_type);

		if (player_1_check && player_2_check)
		{
			win_callback();
		}
	}
}

void CollisionHandler::ChangeVerticalPlatformColor(const Character& player, Platform* platform)
{
	// Set color of vertical platform if there is a collision from the side/underneath 
	if (platform->GetPlatformType() == EPlatformType::kVerticalImpact)
	{
		platform->HandlePlayerCollision(player.GetCharacterType());
	}
}

void CollisionHandler::StopPlayerMovement(const sf::Time dt, Character& player, const PlatformPart& platform_part,
                                          Platform* platform)
{
	//Checks if platform is collidable with player
	if (IsPlayerAtHisPlatform(player, platform))
	{
		// move player out of collision and stop his movement
		player.MoveOutOfCollision(dt, platform_part.GetBoundingRect());
		player.StopMovement();

		ChangeVerticalPlatformColor(player, platform);
	}
}

bool CollisionHandler::CollideAndChangeColors(const sf::Time dt, Character& player, const PlatformPart& platform_part,
                                              Platform* platform)
{
	//Checks if player collided from underneath the center of the platform
	if (IsPlayerAbovePlatform(player, platform_part))
	{
		StopPlayerMovement(dt, player, platform_part, platform);
		// continue to next pair
		return true;
	}
	return false;
}

bool CollisionHandler::PlatformCollision(const sf::Time dt,
                                         SceneNode::Pair pair,
                                         const std::function<void()>& checkpoint_callback,
                                         const std::function<void()>& win_callback,
                                         MultiplayerWorld* multiplayer_world)
{
	if (MatchesCategories(pair, Category::Type::kPlayer, Category::Type::kPlatform))
	{
		auto& player = dynamic_cast<Character&>(*pair.first);
		const auto& platform_part = dynamic_cast<PlatformPart&>(*pair.second);
		Platform* platform = platform_part.GetPlatform();

		if (CollideAndChangeColors(dt, player, platform_part, platform)) return true;

		GroundPlayerAndChangePlatformColor(player, platform, multiplayer_world);
		IsAtTheFinishLine(&player, multiplayer_world->GetTeammate(), checkpoint_callback,
		                  win_callback, platform);
	}
	return false;
}


bool CollisionHandler::IsPlayerAbovePlatform(const Character& player,
                                             const PlatformPart& platform_part)
{
	if (player.getPosition().y > platform_part.getPosition().y)
	{
		return true;
	}

	return false;
}

bool CollisionHandler::IsPlatformStatic(const EPlatformType platform_type)
{
	if (platform_type == EPlatformType::kNormal ||
		platform_type == EPlatformType::kGoal ||
		platform_type == EPlatformType::kCheckpoint ||
		platform_type == EPlatformType::kCheckpointActivated)
	{
		return true;
	}
	return false;
}

bool CollisionHandler::IsPlatformStillChangeable(const EPlatformType platform_type)
{
	return platform_type == EPlatformType::kHorizontalImpact ||
		platform_type == EPlatformType::kVerticalImpact;
}

bool CollisionHandler::IsPlayerAtHisPlatform(const Character& player, const Platform* platform)
{
	const auto platform_type = platform->GetPlatformType();

	if (IsPlatformStatic(platform_type))
		return true;

	if (IsPlatformStillChangeable(platform_type))
		return true;

	return CheckPlatform(platform, player.GetCharacterType());
}


/*
 *	Dylan Goncalves Martins (D00242562)
 *	Adds every collision from one specific ray to a set
 */
void CollisionHandler::GetGroundRayCasts(std::set<SceneNode::Pair>& pairs,
                                         const SceneNode::Pair pair,
                                         const Category::Type category)
{
	if ((pair.first->GetCategory() & category) != 0 ||
		(pair.second->GetCategory() & category) != 0)
	{
		pairs.insert(std::minmax(pair.first, pair.second));
	}
}

/*
 *	Dylan Goncalves Martins (D00242562)
 *	Here we check if one of the pairs is a ray and a platform
 */
void CollisionHandler::PlayerGroundRayCast(const std::set<SceneNode::Pair>& pairs)
{
	bool collide = false;
	SceneNode::Pair player_pair;

	for (SceneNode::Pair pair : pairs)
	{
		player_pair = pair;
		if (MatchesCategories(pair, Category::Type::kRay, Category::Type::kPlatform))
		{
			const auto& ray_ground = dynamic_cast<RayGround&>(*pair.first);
			auto& platform_part = dynamic_cast<PlatformPart&>(*pair.second);
			const auto platform = platform_part.GetPlatform();
			const Character* player = ray_ground.m_character;

			// Check if platform underneath is valid
			if (CheckPlatformUnderneath(player->GetCharacterType(), platform->GetPlatformType()))
			{
				//collision found
				collide = true;
				break;
			}
		}
	}

	// can be null so it jumps out if it happens
	if (player_pair.first == nullptr || player_pair.second == nullptr)
	{
		return;
	}

	// if there was no collision
	if (!collide)
	{
		// check to see which object in pair is the ray 
		if (player_pair.first != nullptr &&
			(player_pair.first->GetCategory() & Category::Type::kRay) != 0)
		{
			//call set falling
			const auto& ray_ground = dynamic_cast<RayGround&>(*player_pair.first);
			ray_ground.SetFalling();
		}
		else if (player_pair.second != nullptr &&
			(player_pair.second->GetCategory() & Category::Type::kRay) != 0)
		{
			const auto& ray_ground = dynamic_cast<RayGround&>(*player_pair.second);
			ray_ground.SetFalling();
		}
	}
}

bool CollisionHandler::CheckPlatformUnderneath(const EColorType color, const EPlatformType platform)
{
	if (IsPlatformStatic(platform))
		return true;

	if (color == EColorType::kRed)
	{
		if (platform == EPlatformType::kVerticalRed ||
			platform == EPlatformType::kHorizontalRed)
		{
			return true;
		}
	}

	if (color == EColorType::kBlue)
	{
		if (platform == EPlatformType::kVerticalBlue ||
			platform == EPlatformType::kHorizontalBlue)
		{
			return true;
		}
	}

	return false;
}

void CollisionHandler::TrapCollision(SceneNode::Pair pair, const std::function<void()>& callback)
{
	//Check Lose Condition
	if (MatchesCategories(pair, Category::Type::kPlayer, Category::Type::kEnemyTrap))
	{
		//Lose
		callback();
	}
}
