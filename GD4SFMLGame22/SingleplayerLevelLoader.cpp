#include "SingleplayerLevelLoader.hpp"
#include "PlatformPart.hpp"

SingleplayerLevelLoader::SingleplayerLevelLoader(LevelManager::LevelData& level_data, TextureHolder& textures, SoundPlayer& sound_player)
	: LevelLoader(level_data, textures, sound_player)
{
}

//Written by Paul Bichler (D00242563)
//This method constructs the level from the CSV files in the level data member field.
//Return a LevelInfo struct that holds all of the information about the level.
SLevelInfo SingleplayerLevelLoader::LoadLevel()
{
	SLevelInfo level_info;
	level_info.background_parent = LoadLevelLayer(m_level_data.m_background_layer_path, level_info, false);
	level_info.level_parent = LoadLevelLayer(m_level_data.m_platform_layer_path, level_info, true);

	return level_info;
}

//Written by Paul Bichler (D00242563)
//Construct a level layer from the file at the specified path
SceneNode::Ptr SingleplayerLevelLoader::LoadLevelLayer(const std::string& csv_path, SLevelInfo& level_info, const bool is_collider_layer)
{
	//Create the root node
	SceneNode::Ptr level_parent = std::make_unique<SceneNode>();
	sf::Vector2f spawn_pos(0.f, 0.f);

	//Convert the csv level data into a 2-dimensional vector (this makes it easier to create platforms)
	m_level_data_vector = LevelDataToVector(csv_path);

	//Loop through the 2-dimensional level vector and construct the level tile by tile
	for (int row = 0; row < m_level_data_vector.size(); row++)
	{
		for (int col = 0; col < m_level_data_vector[row].size(); col++)
		{
			//The id in the level data CSV is converted to a TileType enum to make the code more readable and easily adjustable
			int id = m_level_data_vector[row][col];
			ETileType tile_type = static_cast<ETileType>(id);

			//Construct the tile based on the tile type
			switch (tile_type)
			{
			case kRedPlayer:
			{
				//Create player 1
				std::unique_ptr<Character> player_1(m_tile_factory.CreatePlayer(tile_type, EColorType::kRed, spawn_pos));
				level_info.player_1 = player_1.get();
				level_parent->AttachChild(std::move(player_1));
			}
			break;
			case kBluePlayer:
			{
				//Create player 2
				std::unique_ptr<Character> player_2(m_tile_factory.CreatePlayer(tile_type, EColorType::kBlue, spawn_pos));
				level_info.player_2 = player_2.get();
				level_parent->AttachChild(std::move(player_2));
			}
			break;
			case kHorizontalPlatformPart:
			case kHorizontalImpactPlatformPart:
			case kVerticalImpactPlatformPart:
			case kHorizontalPulsePlatformPart:
			case kVerticalPulsePlatformPart:
			case kHorizontalBluePlatformPart:
			case kHorizontalRedPlatformPart:
			case kVerticalBluePlatformPart:
			case kVerticalRedPlatformPart:
			case kFinishPlatformPart:
				//Construct a platform (the platform type is determined by the tile type)
				CreatePlatform(level_info, tile_type, row, col, level_parent, spawn_pos);
				break;
			case kNone:
				//Tiles marked with (-1 = kNone) do nothing
				break;
			default:
			{
				//Tiles that don't correspond to one of the types above have no special functionality,
				//which is why they are created here with the base "Tile" class.
				SceneNode::Ptr tilePtr(m_tile_factory.CreateTile(tile_type, spawn_pos, is_collider_layer));
				if (tilePtr.get() != nullptr)
					level_parent->AttachChild(std::move(tilePtr));
			}
			}

			spawn_pos.x += m_level_data.m_tile_size.x;
		}

		spawn_pos.x = 0.f;
		spawn_pos.y += m_level_data.m_tile_size.y;
	}

	return level_parent;
}

//Written by Paul Bichler (D00242563)
//This method is called when a platform part is detected and searches for all the other
//platform parts connected to it, to construct a platform.
void SingleplayerLevelLoader::CreatePlatform(SLevelInfo& level_info, const ETileType tile_type,
	const int row, const int col, const SceneNode::Ptr& parent,
	const sf::Vector2f spawn_pos)
{
	std::unique_ptr<Platform> platform(new Platform(static_cast<EPlatformType>(tile_type), m_textures));
	AddPlatformParts(platform.get(), row, col, parent, tile_type, spawn_pos);
	level_info.platforms.emplace_back(std::move(platform));
}

//Written by Paul Bichler (D00242563)
//This method is used to add platform parts to a platform. It loops until no platform part is detected.
void SingleplayerLevelLoader::AddPlatformParts(Platform* platform, const int row, const int col, const SceneNode::Ptr& parent,
	const ETileType tile_type, sf::Vector2f spawn_pos)
{
	const ETileType type = static_cast<ETileType>(m_level_data_vector[row][col]);
	std::unique_ptr<PlatformPart>platform_part(m_tile_factory.CreatePlatformPart(tile_type, spawn_pos, platform));
	parent->AttachChild(std::move(platform_part));

	//Is vertical platform
	for (size_t i = row + 1; i < m_level_data_vector.size(); i++)
	{
		//Stop the loop when the tile doesn't belong to the platform
		if (static_cast<ETileType>(m_level_data_vector[i][col]) != type)
			break;

		spawn_pos.y += m_level_data.m_tile_size.y;
		std::unique_ptr<PlatformPart> vertical_platform_part(m_tile_factory.CreatePlatformPart(tile_type, spawn_pos, platform));
		parent->AttachChild(std::move(vertical_platform_part));

		//Set the tile type to -1 to prevent duplication
		m_level_data_vector[i][col] = -1;
	}

	//Is horizontal platform
	for (size_t j = col + 1; j < m_level_data_vector[row].size(); j++)
	{
		//Stop the loop when the tile doesn't belong to the platform
		if (static_cast<ETileType>(m_level_data_vector[row][j]) != type)
			break;

		spawn_pos.x += m_level_data.m_tile_size.x;
		std::unique_ptr<PlatformPart> horizontal_platform_part(m_tile_factory.CreatePlatformPart(tile_type, spawn_pos, platform));
		parent->AttachChild(std::move(horizontal_platform_part));

		//Set the tile type to -1 to prevent duplication
		m_level_data_vector[row][j] = -1;
	}
}
