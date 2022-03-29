#include "CollisionHandler.hpp"

#include "Character.hpp"


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

bool CollisionHandler::CollideFromAbove(Character& player, Tile& tile)
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

bool CollisionHandler::HandlePlayerTileCollision(SceneNode::Pair pair)
{
	if (MatchesCategories(pair, Category::Type::kPlayer, Category::Type::kPlatform))
	{
		auto& player = dynamic_cast<Character&>(*pair.first);
		auto& tile = dynamic_cast<Tile&>(*pair.second);

		if (CollideFromAbove(player, tile)) return true;

		GroundPlayer(player, tile);
	}
	return false;
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
