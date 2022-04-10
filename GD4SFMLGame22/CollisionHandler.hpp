#pragma once
#include "Character.hpp"
#include <utility>
#include "MultiplayerWorld.hpp"
#include "PlatformPart.hpp"

class CollisionHandler
{
	static bool CheckPlatform(const Platform* platform, EColorType character);
	static bool IsVerticalPlatform(EPlatformType platform_type);
	static bool MatchesCategories(SceneNode::Pair& collision, Category::Type type1, Category::Type type2);
	static void IsAtTheFinishLine(const Character* player_1, const Character* player_2,
	                              const std::function<void()>& checkpoint_callback,
	                              const std::function<void()>& win_callback, const Platform* platform);
	static void ChangeVerticalPlatformColor(const Character& player, Platform* platform);
	static void StopPlayerMovement(sf::Time dt, Character& player, const PlatformPart& platform_part,
	                               Platform* platform);
	static bool CollideAndChangeColors(sf::Time dt, Character& player, const PlatformPart& platform_part,
	                                   Platform* platform);
	static bool IsPlayerAbovePlatform(const Character& player, const PlatformPart& platform_part);
	static bool IsPlatformStatic(EPlatformType platform_type);
	static bool IsPlatformStillChangeable(EPlatformType platform_type);
	static bool IsPlayerAtHisPlatform(const Character& player, const Platform* platform);
	static bool CheckPlatformUnderneath(EColorType color, EPlatformType platform);
	static void GroundPlayerAndChangePlatformColor(Character& player, Platform* platform,
	                                               MultiplayerWorld* multiplayer_world = nullptr);

public:
	static bool PlatformCollision(sf::Time dt, SceneNode::Pair pair, const std::function<void()>& checkpoint_callback,
	                              const std::function<void()>& win_callback,
	                              MultiplayerWorld* multiplayer_world = nullptr);
	static void GetGroundRayCasts(std::set<SceneNode::Pair>& pairs, SceneNode::Pair pair,
	                              Category::Type category);
	static void PlayerGroundRayCast(const std::set<SceneNode::Pair>& pairs);
	static void TrapCollision(SceneNode::Pair pair, const std::function<void()>& callback);
};
