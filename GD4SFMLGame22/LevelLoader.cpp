//Written by Paul Bichler (D00242563)

#include "LevelLoader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

#include "PlatformPart.hpp"
#include "ResourceHolder.hpp"

//Written by Paul Bichler (D00242563)
//The LevelLoader class is used to construct levels based on the level data CSV files in the LevelManager.
//It uses the TileFactory class to create instances for the tile types specified in the level files.
LevelLoader::LevelLoader(LevelManager::LevelData& level_data, TextureHolder& textures, FontHolder& fonts, SoundPlayer& sound_player)
	: m_level_data(level_data),
	  m_textures(textures),
	  m_tile_factory(textures, fonts, level_data.m_tile_size, sound_player)
{
}

//Written by Paul Bichler (D00242563)
//This method constructs the level from the CSV files in the level data member field.
//Return a LevelInfo struct that holds all of the information about the level.
LevelInfo LevelLoader::LoadLevel()
{
	LevelInfo level_info;
	level_info.background_parent = LoadLevelLayer(m_level_data.m_background_layer_path, level_info, false);
	level_info.level_parent = LoadLevelLayer(m_level_data.m_platform_layer_path, level_info, true);

	return level_info;
}

//Written by Paul Bichler (D00242563)
//Construct a level layer from the file at the specified path
SceneNode::Ptr LevelLoader::LoadLevelLayer(const std::string& csv_path, LevelInfo& level_info, const bool is_collider_layer)
{
	//Create the root node
	SceneNode::Ptr level_parent = std::make_unique<SceneNode>();
	sf::Vector2f spawn_pos(0.f, 0.f);

	//Convert the csv level data into a 2-dimensional vector (this makes it easier to create platforms)
	m_level_data_vector = LevelDataToVector(csv_path);

	sf::Int8 platform_id = 0;

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
				//Do not swap these lines as for red the spawn position is set in GetSubRect
				level_info.m_red_player_rect = m_tile_factory.GetSubRect(kRedPlayer, spawn_pos);
				level_info.m_red_player_spawn_pos = spawn_pos;
			}
			break;
			case kBluePlayer:
			{
				//Do not swap these lines as for blue the spawn position is set before GetSubRect
				level_info.m_blue_player_spawn_pos = spawn_pos;
				level_info.m_blue_player_rect = m_tile_factory.GetSubRect(kBluePlayer, spawn_pos);
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
			case kCheckpointPlatformPart:
				//Construct a platform (the platform type is determined by the tile type)
				{
					//Construct a platform (the platform type is determined by the tile type)
					CreatePlatform(level_info, tile_type, row, col, level_parent, spawn_pos, platform_id++);
				}
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
//This method reads a csv level file line-by-line and converts it to a 2-dimensional vector
std::vector<std::vector<int>> LevelLoader::LevelDataToVector(const std::string& csv_path) const
{
	std::vector<std::vector<int>> levelDataVector;
	std::ifstream in(csv_path);
	assert(!in.fail());

	std::string line;
	while (std::getline(in, line))
	{
		std::vector<int> levelLineData;

		std::stringstream ss(line);
		std::string token;

		while (std::getline(ss, token, ','))
			levelLineData.emplace_back(std::stoi(token));

		levelDataVector.emplace_back(levelLineData);
	}

	return levelDataVector;
}

//Written by Paul Bichler (D00242563)
//This method is called when a platform part is detected and searches for all the other
//platform parts connected to it, to construct a platform.
void LevelLoader::CreatePlatform(LevelInfo& level_info, const ETileType tile_type,
	const int row, const int col, SceneNode::Ptr& parent,
	const sf::Vector2f spawn_pos, const sf::Int8 platform_id)
{
	std::unique_ptr<Platform> platform(new Platform(platform_id, static_cast<EPlatformType>(tile_type), m_textures));
	AddPlatformParts(platform.get(), row, col, parent, tile_type, spawn_pos);
	level_info.platforms.emplace_back(std::move(platform));
}

//Written by Paul Bichler (D00242563)
//This method is used to add platform parts to a platform. It loops until no platform part is detected.
void LevelLoader::AddPlatformParts(Platform* platform, const int row, const int col, SceneNode::Ptr& parent,
	const ETileType tile_type, sf::Vector2f spawn_pos)
{
	const ETileType type = static_cast<ETileType>(m_level_data_vector[row][col]);

	std::unique_ptr<PlatformPart> platform_part(m_tile_factory.CreatePlatformPart(tile_type, spawn_pos, platform));
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
