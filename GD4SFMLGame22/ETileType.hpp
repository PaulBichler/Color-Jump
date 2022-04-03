//Written by Paul Bichler (D00242563)

#pragma once

//The values for these enums represent the position of the appropriate tile in the tileset the level is constructed from.
//These value can also be found in the LevelData CSV files (the level data uses -1 as an empty tile).
enum ETileType
{
	kNone = -1,
	kBluePlayer = 5,
	kRedPlayer = 6,
	kHorizontalPlatformPart = 9,
	kHorizontalRedPlatformPart = 17,
	kHorizontalBluePlatformPart = 15,
	kVerticalRedPlatformPart = 11,
	kVerticalBluePlatformPart = 8,
	kHorizontalImpactPlatformPart = 13,
	kVerticalImpactPlatformPart = 7,
	kHorizontalPulsePlatformPart = 16,
	kVerticalPulsePlatformPart = -4,
	kFinishPlatformPart = 18,
	kCheckpointPlatformPart = 14,
	kSpikes = 12
};
