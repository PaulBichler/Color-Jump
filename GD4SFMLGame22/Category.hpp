#pragma once
//SceneNode category used to dispatch commands
namespace Category
{
	enum Type
	{
		kNone = 0,
		kScene = 1 << 0,
		kGhost = 1 << 1,
		kParticleSystem = 1 << 2,
		kSoundEffect = 1 << 3,
		kPlayerOne = 1 << 4,
		kPlayerTwo = 1 << 5,
		kPlatform = 1 << 6,
		kBluePlatform = 1 << 7,
		kRedPlatform = 1 << 8,
		kRayOne = 1 << 9,
		kRayTwo = 1 << 10,
		kEnemyTrap = 1 << 11,
		
		kPlayer = kPlayerOne | kPlayerTwo,
		kRay = kRayOne | kRayTwo,
	};
}
