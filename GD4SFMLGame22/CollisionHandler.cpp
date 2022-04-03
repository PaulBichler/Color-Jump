#include "CollisionHandler.hpp"

#include "Character.hpp"
#include "MultiplayerGameState.hpp"
#include "PlatformPart.hpp"
#include "Utility.hpp"


void CollisionHandler::ChangeVerticalTileColor(const Character& player, Tile& tile)
{
	// Set color of vertical platform if there is a collision from the side/underneath 
	if (tile.GetType() == kVerticalImpactPlatformPart)
	{
		tile.HandleCollision(player.GetCharacterType());
	}
}

void CollisionHandler::StopPlayerMovement(Character& player, Tile& tile)
{
	//Checks if platform is collidable with player
	if (IsPlayerAtHisTile(player, tile))
	{
		// move player out of collision and stop his movement
		player.MoveOutOfCollision(tile.GetBoundingRect());
		player.StopMovement();

		ChangeVerticalTileColor(player, tile);
	}
}


bool CollisionHandler::IsPlayerAtHisTile(const Character& player, const Tile& tile)
{
	if (tile.GetType() == kHorizontalPlatformPart || tile.GetType() == kFinishPlatformPart)
	{
		return true;
	}

	if (tile.GetType() == kHorizontalImpactPlatformPart || tile.GetType() ==
		kVerticalImpactPlatformPart)
	{
		return true;
	}


	if (player.GetCharacterType() == EColorType::kBlue)
	{
		return CheckTile(tile, EColorType::kBlue);
	}

	if (player.GetCharacterType() == EColorType::kRed)
	{
		return CheckTile(tile, EColorType::kRed);
	}

	return false;
}

bool CollisionHandler::CheckPlatform(const Platform* platform, const EColorType character)
{
	if (character == EColorType::kBlue)
	{
		if (platform->GetPlatformType() == EPlatformType::kHorizontalBlue || platform->
			GetPlatformType() ==
			EPlatformType::kVerticalBlue)
		{
			return true;
		}
	}
	else if (character == EColorType::kRed)
	{
		if (platform->GetPlatformType() == EPlatformType::kHorizontalRed || platform->
			GetPlatformType() ==
			EPlatformType::kVerticalRed)
		{
			return true;
		}
	}

	return false;
}

bool CollisionHandler::CheckTile(const Tile& tile, const EColorType character)
{
	if (character == EColorType::kBlue)
	{
		if (tile.GetType() == kHorizontalBluePlatformPart || tile.GetType() ==
			kVerticalBluePlatformPart)
		{
			return true;
		}
	}
	else if (character == EColorType::kRed)
	{
		if (tile.GetType() == kHorizontalRedPlatformPart || tile.GetType() ==
			kVerticalRedPlatformPart)
		{
			return true;
		}
	}

	return false;
}

bool CollisionHandler::CollideAndChangeColors(Character& player, Tile& tile)
{
	//Checks if player collided from underneath the center of the platform
	if (IsPlayerAboveTile(player, tile))
	{
		StopPlayerMovement(player, tile);
		// continue to next pair
		return true;
	}
	return false;
}

bool CollisionHandler::IsPlayerAboveTile(const Character& player, const Tile& tile)
{
	if (player.getPosition().y > tile.getPosition().y)
	{
		return true;
	}

	return false;
}


void CollisionHandler::GroundPlayer(Character& player, Tile& tile)
{
	//Ground players
	if (tile.HandleCollision(player.GetCharacterType()))
	{
		//Collision
		player.SetGrounded();
	}
}

void CollisionHandler::GroundPlayerAndChangePlatformColor(Character& player, Platform* platform,
                                                          MultiplayerWorld* multiplayer_world)
{
	//Ground players
	if (platform->HandlePlayerCollisionAndChangeColor(player.GetCharacterType()))
	{
		if (multiplayer_world != nullptr)
		{
			multiplayer_world->m_state->SendPlatformInfo(player.GetTeamIdentifier(),
			                                             platform->GetID(),
			                                             platform->GetPlatformType());
		}
		//Collision
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

void CollisionHandler::GroundPlayer(Character& player, Platform* platform)
{
	if (platform->HandlePlayerCollision(player.GetCharacterType()))
	{
		player.SetGrounded(platform);
	}
}

void CollisionHandler::IsAtTheFinishLine(const std::vector<Character*>& players,
                                         const std::function<void()>
                                         & callback, const Platform* platform,
                                         const sf::Int8 team_id)
{
	EPlatformType platform_type = platform->GetPlatformType();

	//Check Win Condition
	if (platform_type == EPlatformType::kGoal || platform_type == EPlatformType::kCheckpoint)
	{
		bool is_playing = false;
		for (const auto character : players)
		{
			if (character->GetTeamIdentifier() == team_id)
			{
				if (!character->IsOnPlatformOfType(EPlatformType::kGoal))
				{
					is_playing = true;
				}
			}
		}

		if (!is_playing)
		{
			callback();
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

void CollisionHandler::StopPlayerMovement(Character& player, const PlatformPart& platform_part, Platform* platform)
{
	//Checks if platform is collidable with player
	if (IsPlayerAtHisPlatform(player, platform))
	{
		// move player out of collision and stop his movement
		player.MoveOutOfCollision(platform_part.GetBoundingRect());
		player.StopMovement();

		ChangeVerticalPlatformColor(player, platform);
	}
}

bool CollisionHandler::CollideAndChangeColors(Character& player, const PlatformPart& platform_part,
                                              Platform* platform)
{
	//Checks if player collided from underneath the center of the platform
	if (IsPlayerAbovePlatform(player, platform_part))
	{
		StopPlayerMovement(player, platform_part, platform);
		// continue to next pair
		return true;
	}
	return false;
}

bool CollisionHandler::PlatformCollision(SceneNode::Pair pair,
                                         const std::vector<Character*>& players,
                                         const std::function<void()>& callback,
                                         MultiplayerWorld* multiplayer_world)
{
	if (MatchesCategories(pair, Category::Type::kPlayer, Category::Type::kPlatform))
	{
		auto& player = dynamic_cast<Character&>(*pair.first);
		const auto& platform_part = dynamic_cast<PlatformPart&>(*pair.second);
		Platform* platform = platform_part.GetPlatform();

		if (CollideAndChangeColors(player, platform_part, platform)) return true;

		GroundPlayerAndChangePlatformColor(player, platform, multiplayer_world);
		IsAtTheFinishLine(players, callback, platform, player.GetTeamIdentifier());
	}

	if (MatchesCategories(pair, Category::Type::kGhost, Category::Type::kPlatform))
	{
		auto& player = dynamic_cast<Character&>(*pair.first);
		const auto& platform_part = dynamic_cast<PlatformPart&>(*pair.second);
		Platform* platform = platform_part.GetPlatform();

		if (Collide(player, platform_part, platform)) return true;

		GroundPlayer(player, platform);
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

bool CollisionHandler::IsPlayerAtHisPlatform(const Character& player, const Platform* platform)
{
	if (platform->GetPlatformType() == EPlatformType::kNormal || platform->GetPlatformType() ==
		EPlatformType::kGoal)
	{
		return true;
	}

	if (platform->GetPlatformType() == EPlatformType::kHorizontalImpact
		|| platform->GetPlatformType() == EPlatformType::kVerticalImpact)
	{
		return true;
	}


	if (player.GetCharacterType() == EColorType::kBlue)
	{
		return CheckPlatform(platform, EColorType::kBlue);
	}

	if (player.GetCharacterType() == EColorType::kRed)
	{
		return CheckPlatform(platform, EColorType::kRed);
	}

	return false;
}


/*
 *	Dylan Goncalves Martins (D00242562)
 *	Adds every collision from one specific ray to a set
 */
void CollisionHandler::GetGroundRayCasts(std::set<SceneNode::Pair>& pairs,
                                         const SceneNode::Pair pair,
                                         const Category::Type category)
{
	if (pair.first->GetCategory() == category || pair.second->GetCategory() == category)
	{
		pairs.insert(std::minmax(pair.first, pair.second));
	}
}

/*
 *	Dylan Goncalves Martins (D00242562)
 *	Here is checked if one of the pairs is a ray and a platform
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
		if (player_pair.first != nullptr && (player_pair.first->GetCategory() &
			Category::Type::kRay) != 0)
		{
			//call set falling
			const auto& ray_ground = dynamic_cast<RayGround&>(*player_pair.first);
			ray_ground.SetFalling();
		}
		else if (player_pair.second != nullptr && (player_pair.second->GetCategory() &
			Category::Type::kRay) != 0)
		{
			const auto& ray_ground = dynamic_cast<RayGround&>(*player_pair.second);
			ray_ground.SetFalling();
		}
	}
}

bool CollisionHandler::CheckPlatformUnderneath(const EColorType color, const EPlatformType platform)
{
	if (platform == EPlatformType::kGoal || platform == EPlatformType::kNormal)
	{
		return true;
	}

	if (color == EColorType::kRed)
	{
		if (platform == EPlatformType::kVerticalRed || platform == EPlatformType::kHorizontalRed)
		{
			return true;
		}
	}

	if (color == EColorType::kBlue)
	{
		if (platform == EPlatformType::kVerticalBlue || platform == EPlatformType::kHorizontalBlue)
		{
			return true;
		}
	}

	return false;
}

bool CollisionHandler::Collide(Character& character, const PlatformPart& platform_part,
                               Platform* platform)
{
	//Checks if player collided from underneath the center of the platform
	if (IsPlayerAbovePlatform(character, platform_part))
	{
		StopPlayerMovement(character, platform_part, platform);
		// continue to next pair
		return true;
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

void CollisionHandler::GetGroundRayCasts(std::map<Character*, std::set<SceneNode::Pair>>& sets,
                                         const SceneNode::Pair& pair, const Category::Type category)
{
	if ((pair.first->GetCategory() & category) != 0)
	{
		const auto& ray_ground = dynamic_cast<RayGround&>(*pair.first);

		const auto it = sets.find(ray_ground.m_character);
		if (it != sets.end())
		{
			sets[ray_ground.m_character].insert(std::minmax(pair.first, pair.second));
		}
	}

	if ((pair.second->GetCategory() & category) != 0)
	{
		const auto& ray_ground = dynamic_cast<RayGround&>(*pair.second);

		const auto it = sets.find(ray_ground.m_character);
		if (it != sets.end())
		{
			sets[ray_ground.m_character].insert(std::minmax(pair.first, pair.second));
		}
	}
}

void CollisionHandler::PlayerGroundRayCast(
	const std::map<Character*, std::set<SceneNode::Pair>>& map)
{
	bool collide = false;
	SceneNode::Pair player_pair;

	for (auto& value : map)
	{
		for (SceneNode::Pair pair : value.second)
		{
			player_pair = pair;
			if (MatchesCategories(pair, Category::Type::kRay, Category::Type::kPlatform))
			{
				const auto& ray_ground = dynamic_cast<RayGround&>(*pair.first);
				auto& platform_part = dynamic_cast<PlatformPart&>(*pair.second);
				const auto platform = platform_part.GetPlatform();
				const Character* player = ray_ground.m_character;

				// Check if platform underneath is valid
				if (CheckPlatformUnderneath(player->GetCharacterType(),
				                            platform->GetPlatformType()))
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
			continue;
		}

		// if there was no collision
		if (!collide)
		{
			// check to see which object in pair is the ray 
			if (player_pair.first != nullptr && (player_pair.first->GetCategory() &
				Category::Type::kRay) != 0)
			{
				//call set falling
				const auto& ray_ground = dynamic_cast<RayGround&>(*player_pair.first);
				ray_ground.SetFalling();
			}
			else if (player_pair.second != nullptr && (player_pair.second->GetCategory() &
				Category::Type::kRay) != 0)
			{
				const auto& ray_ground = dynamic_cast<RayGround&>(*player_pair.second);
				ray_ground.SetFalling();
			}
		}
	}
}
