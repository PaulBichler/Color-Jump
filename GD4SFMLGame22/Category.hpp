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
		kParticleSystem = 1 << 7,
		kSoundEffect = 1 << 8,
		kPlayerOne = 1 << 9,
		kPlayerTwo = 1 << 10,
		kPlatform = 1 << 11,
		kBluePlatform = 1 << 12,
		kRedPlatform = 1 << 13,
		kRayOne = 1 << 14,
		kRayTwo = 1 << 15,
		kEnemyTrap = 1 << 16,
		kNetwork = 1 << 17,

		kAircraft = kPlayerAircraft | kAlliedAircraft | kEnemyAircraft,
		kPlayer = kPlayerOne | kPlayerTwo,
		kRay = kRayOne | kRayTwo,
		kProjectile = kAlliedProjectile | kEnemyProjectile,
	};
}
