//Written by Paul Bichler (D00242563)

#pragma once
#include "ETileType.hpp"

//Each Platform in the game has a type to determine what happens when the player collides with it.
//The values of the Platform types depend on the platform part tile type it's associated with.
enum class EPlatformType
{
	kNormal = kHorizontalPlatformPart,
	kHorizontalImpact = kHorizontalImpactPlatformPart,
	kVerticalImpact = kVerticalImpactPlatformPart,
	kHorizontalRed = kHorizontalRedPlatformPart,
	kHorizontalBlue = kHorizontalBluePlatformPart,
	kVerticalRed = kVerticalRedPlatformPart,
	kVerticalBlue = kVerticalBluePlatformPart,
	kHorizontalPulse = kHorizontalPulsePlatformPart,
	kVerticalPulse = kVerticalPulsePlatformPart,
	kGoal = kFinishPlatformPart,
	kCheckpoint = kCheckpointPlatformPart,
	kCheckpointActivated = -kCheckpointPlatformPart
};
