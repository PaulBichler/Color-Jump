#pragma once
#include "Character.hpp"
#include <utility>
#include "Character.hpp"
#include "MultiplayerWorld.hpp"
#include "PlatformPart.hpp"

class CollisionHandler
{
	static void ChangeVerticalTileColor(const Character& player, Tile& tile);
	static void StopPlayerMovement(Character& player, Tile& tile);
	static bool IsPlayerAtHisTile(const Character& player, const Tile& tile);
	static bool CheckPlatform(const Platform* platform, EColorType character);
	static bool CheckTile(const Tile& tile, EColorType character);
	static bool CollideAndChangeColors(Character& player, Tile& tile);
	static bool IsPlayerAboveTile(const Character& player, const Tile& tile);
	static void GroundPlayer(Character& player, Tile& tile);
	static bool MatchesCategories(SceneNode::Pair& collision, Category::Type type1,
	                              Category::Type type2);
	static void GroundPlayer(Character& player, Platform* platform);
	static void IsAtTheFinishLine(const Character* player_1, const Character* player_2, const std::function<void()>& callback, const Platform* platform);
	static void ChangeVerticalPlatformColor(const Character& player, Platform* platform);
	static void StopPlayerMovement(Character& player, const PlatformPart& platform_part,
	                               Platform* platform);
	static bool CollideAndChangeColors(Character& player, const PlatformPart& platform_part,
	                                   Platform* platform);
	static bool IsPlayerAbovePlatform(const Character& player, const PlatformPart& platform_part);
	static bool IsPlayerAtHisPlatform(const Character& player, const Platform* platform);
	static bool CheckPlatformUnderneath(EColorType color, EPlatformType platform);
	static bool Collide(Character& character, const PlatformPart& platform_part,
	                    Platform* platform);

public:
	static bool TileCollision(SceneNode::Pair pair);
	static void GroundPlayerAndChangePlatformColor(Character& player, Platform* platform, MultiplayerWorld* multiplayer_world = nullptr);
	static bool PlatformCollision(SceneNode::Pair pair,
	                              const std::function<void()>& callback,
	                              MultiplayerWorld* multiplayer_world = nullptr);
	static void GetGroundRayCasts(std::set<SceneNode::Pair>& pairs, SceneNode::Pair pair,
	                              Category::Type category);
	static void PlayerGroundRayCast(const std::set<SceneNode::Pair>& pairs);
	static void TrapCollision(SceneNode::Pair pair, const std::function<void()>& callback);
	static void GetGroundRayCasts(std::map<Character*, std::set<SceneNode::Pair>>& sets,
	                              const SceneNode::Pair& pair, Category
	                              ::Type category);
	static void PlayerGroundRayCast(const std::map<Character*, std::set<SceneNode::Pair>>& map);
};
