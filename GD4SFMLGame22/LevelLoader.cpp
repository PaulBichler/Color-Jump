#include "LevelLoader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <SFML/Graphics/View.hpp>

#include "ETileType.hpp"
#include "PlatformPart.hpp"
#include "ResourceHolder.hpp"

LevelLoader::LevelLoader(TextureHolder& textures)
	: m_textures(textures)
{
}

LevelLoader::LevelInfo LevelLoader::LoadLevel(const LevelManager::LevelData& level_data) const
{
	LevelInfo level_info;
	level_info.tile_size = level_data.m_tile_size;
	level_info.background_parent = LoadLevelLayer(level_info, level_data.m_background_layer_path);
	level_info.level_parent = LoadLevelLayer(level_info, level_data.m_platform_layer_path);

	return level_info;
}

SceneNode::Ptr LevelLoader::LoadLevelLayer(LevelInfo& level_info, const std::string& csv_path) const
{
	SceneNode::Ptr level_parent = std::make_unique<SceneNode>();
	TileFactory tile_factory(m_textures, level_info.tile_size);
	sf::Vector2f spawn_pos(0.f, 0.f);

	std::vector<std::vector<int>> level_data = LevelDataToVector(csv_path);

	for(int row = 0; row < level_data.size(); row++)
	{
		for(int col = 0; col < level_data[row].size(); col++)
		{
			int id = level_data[row][col];
			ETileType tile_type = static_cast<ETileType>(id);

			if(tile_type == kRedPlayer)
			{
				std::unique_ptr<Character> player_1(tile_factory.CreatePlayer(id, ECharacterType::kRed));
				level_parent->AttachChild(std::move(player_1));
				level_info.player_1 = player_1.get();
			}
			else if(tile_type == kBluePlayer)
			{
				std::unique_ptr<Character> player_2(tile_factory.CreatePlayer(id, ECharacterType::kBlue));
				level_parent->AttachChild(std::move(player_2));
				level_info.player_2 = player_2.get();
			}
			else if(tile_type == kPlatform)
			{
				CreatePlatform(level_info, level_data, row, col, level_parent, tile_factory, spawn_pos);
			}
			else if(tile_type != kNone)
			{
				SceneNode::Ptr tilePtr(tile_factory.CreateTile(id, spawn_pos));
				if(tilePtr.get() != nullptr)
					level_parent->AttachChild(std::move(tilePtr));
			}


			spawn_pos.x += level_info.tile_size.x;
		}

		spawn_pos.x = 0.f;
		spawn_pos.y += level_info.tile_size.y;
	}

	return level_parent;
}

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
		{
			levelLineData.emplace_back(std::stoi(token));
		}

		levelDataVector.emplace_back(levelLineData);
	}

	return levelDataVector;
}

void LevelLoader::CreatePlatform(LevelInfo& level_info, std::vector<std::vector<int>>& level_data, int row, int col, SceneNode::Ptr& parent, TileFactory& tile_factory, sf::Vector2f spawn_pos) const
{
	std::unique_ptr<Platform> platform(new Platform());

	ETileType type = static_cast<ETileType>(level_data[row][col]);

	std::unique_ptr<PlatformPart> platform_part(tile_factory.CreatePlatformPart(type, spawn_pos, platform.get()));
	parent->AttachChild(std::move(platform_part));

	//Is vertical platform
	for(size_t i = row + 1; i < level_data.size(); i++)
	{
		if(static_cast<ETileType>(level_data[i][col]) != type)
			break;

		spawn_pos.y += level_info.tile_size.y;
		std::unique_ptr<PlatformPart> vertical_platform_part(tile_factory.CreatePlatformPart(type, spawn_pos, platform.get()));
		parent->AttachChild(std::move(vertical_platform_part));
		level_data[i][col] = -1;
	}

	//Is horizontal platform
	for(size_t j = col + 1; j < level_data[row].size(); j++)
	{
		if(static_cast<ETileType>(level_data[row][j]) != type)
			break;

		spawn_pos.x += level_info.tile_size.x;
		std::unique_ptr<PlatformPart> horizontal_platform_part(tile_factory.CreatePlatformPart(type, spawn_pos, platform.get()));
		parent->AttachChild(std::move(horizontal_platform_part));
		level_data[row][j] = -1;
	}

	level_info.platforms.emplace_back(std::move(platform));
	std::cout << level_info.platforms.size() << std::endl;
}
