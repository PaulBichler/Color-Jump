#pragma once
#include "Character.hpp"

class CollisionHandler
{
	static void ChangeVerticalTileColor(const Character& player, Tile& tile);
	static void StopPlayerMovement(Character& player, Tile& tile);
	static bool IsPlayerAtHisTile(const Character& player, const Tile& tile);
	static bool CheckTile(const Tile& tile, EColorType character);
	static bool CollideFromAbove(Character& player, Tile& tile);
	static bool IsPlayerAboveTile(const Character& player, const Tile& tile);
	static void GroundPlayer(Character& player, Tile& tile);
	static bool MatchesCategories(SceneNode::Pair& collision, Category::Type type1, Category::Type type2);
public:
	static bool HandlePlayerTileCollision(SceneNode::Pair pair, Platform*& collided_platform);
};

