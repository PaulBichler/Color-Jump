#pragma once
//SceneNode category used to dispatch commands
namespace Category
{
	enum Type
	{
		kNone = 0,
		kScene = 1 << 0,
		kPlayerAircraft = 1 << 1,
		kAlliedAircraft = 1 << 2,
		kEnemyAircraft = 1 << 3,
		kPickup = 1 << 4,
		kAlliedProjectile = 1 << 5,
		kEnemyProjectile = 1 << 6,
		kPlayerOne = 1 << 7,
		kPlayerTwo = 1 << 8,
		kPlatform = 1 << 9,
		kBluePlatform = 1 << 10,
		kRedPlatform = 1 << 11,

		kAircraft = kPlayerAircraft | kAlliedAircraft | kEnemyAircraft,
		kPlayer = kPlayerOne | kPlayerTwo,
		kProjectile = kAlliedProjectile | kEnemyProjectile,
	};
}