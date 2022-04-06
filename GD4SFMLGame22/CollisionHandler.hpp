#pragma once
#include "Character.hpp"
#include <utility>
#include "MultiplayerWorld.hpp"
#include "PlatformPart.hpp"

class CollisionHandler
{
	static void ChangeVerticalTileColor(const Character& player, Tile& tile);
	static void StopPlayerMovement(const sf::Time dt, Character& player, Tile& tile);
	static bool IsPlayerAtHisTile(const Character& player, const Tile& tile);
	static bool CheckPlatform(const Platform* platform, EColorType character);
	static bool CheckTile(const Tile& tile, EColorType character);
	static bool CollideAndChangeColors(const sf::Time dt, Character& player, Tile& tile);
	static bool IsPlayerAboveTile(const Character& player, const Tile& tile);
	static void GroundPlayer(Character& player, Tile& tile);
	static bool MatchesCategories(SceneNode::Pair& collision, Category::Type type1,
	                              Category::Type type2);
	static void GroundPlayer(Character& player, Platform* platform);
	static void IsAtTheFinishLine(const Character* player_1, const Character* player_2, const std::function<void()>&
	                              checkpoint_callback, const std::function<void()>& win_callback, const Platform* platform);
	static void ChangeVerticalPlatformColor(const Character& player, Platform* platform);
	static void StopPlayerMovement(const sf::Time dt, Character& player,
	                               const PlatformPart& platform_part, Platform* platform);
	static bool CollideAndChangeColors(const sf::Time dt, Character& player,
	                                   const PlatformPart& platform_part, Platform* platform);
	static bool IsPlayerAbovePlatform(const Character& player, const PlatformPart& platform_part);
	static bool IsPlayerAtHisPlatform(const Character& player, const Platform* platform);
	static bool CheckPlatformUnderneath(EColorType color, EPlatformType platform);
	static bool Collide(const sf::Time dt, Character& character,
	                    const PlatformPart& platform_part, Platform* platform);

public:
	static void GroundPlayerAndChangePlatformColor(Character& player, Platform* platform, MultiplayerWorld* multiplayer_world = nullptr);
	static bool PlatformCollision(const sf::Time dt,
	                              SceneNode::Pair pair,
	                              const std::function<void()>& checkpoint_callback, const std::function<void()>& win_callback, MultiplayerWorld
	                              * multiplayer_world = nullptr);
	static void GetGroundRayCasts(std::set<SceneNode::Pair>& pairs, SceneNode::Pair pair,
	                              Category::Type category);
	static void PlayerGroundRayCast(const std::set<SceneNode::Pair>& pairs);
	static void TrapCollision(SceneNode::Pair pair, const std::function<void()>& callback);
	static void GetGroundRayCasts(std::map<Character*, std::set<SceneNode::Pair>>& sets,
	                              const SceneNode::Pair& pair, Category
	                              ::Type category);
	static void PlayerGroundRayCast(const std::map<Character*, std::set<SceneNode::Pair>>& map);
};
