#include "LevelLoader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <SFML/Graphics/View.hpp>

#include "ETileType.hpp"
#include "PlatformPart.hpp"
#include "ResourceHolder.hpp"

LevelLoader::LevelLoader(LevelManager::LevelData& level_data, TextureHolder& textures)
	: m_level_data(level_data),
	m_textures(textures),
	m_tile_factory(textures, level_data.m_tile_size)
{
}

LevelLoader::LevelInfo LevelLoader::LoadLevel()
{
	LevelInfo level_info;
	level_info.background_parent = LoadLevelLayer(level_info, m_level_data.m_background_layer_path, false);
	level_info.level_parent = LoadLevelLayer(level_info, m_level_data.m_platform_layer_path, true);

	return level_info;
}

SceneNode::Ptr LevelLoader::LoadLevelLayer(LevelInfo& level_info, const std::string& csv_path, const bool is_collider_layer)
{
	SceneNode::Ptr level_parent = std::make_unique<SceneNode>();
	sf::Vector2f spawn_pos(0.f, 0.f);

	m_level_data_vector = LevelDataToVector(csv_path);

	for(int row = 0; row < m_level_data_vector.size(); row++)
	{
		for(int col = 0; col < m_level_data_vector[row].size(); col++)
		{
			int id = m_level_data_vector[row][col];
			ETileType tile_type = static_cast<ETileType>(id);

			if(tile_type == kRedPlayer)
			{
				std::unique_ptr<Character> player_1(m_tile_factory.CreatePlayer(id, tile_type, ECharacterType::kRed, spawn_pos));
				level_info.player_1 = player_1.get();
				level_parent->AttachChild(std::move(player_1));
			}
			else if(tile_type == kBluePlayer)
			{
				std::unique_ptr<Character> player_2(m_tile_factory.CreatePlayer(id, tile_type, ECharacterType::kBlue, spawn_pos));
				level_info.player_2 = player_2.get();
				level_parent->AttachChild(std::move(player_2));
			}
			else if(tile_type == kPlatform)
			{
				CreatePlatform(EPlatformType::kNormal, tile_type, level_info, row, col, level_parent, spawn_pos);
			}
			else if(tile_type == kImpactPlatform)
			{
				CreatePlatform(EPlatformType::kImpact, tile_type, level_info, row, col, level_parent, spawn_pos);
			}
			else if(tile_type == kFinish)
			{
				CreatePlatform(EPlatformType::kGoal, tile_type, level_info, row, col, level_parent, spawn_pos);
			}
			else if(tile_type != kNone)
			{
				SceneNode::Ptr tilePtr(m_tile_factory.CreateTile(id, spawn_pos, tile_type, is_collider_layer));
				if(tilePtr.get() != nullptr)
					level_parent->AttachChild(std::move(tilePtr));
			}


			spawn_pos.x += m_level_data.m_tile_size.x;
		}

		spawn_pos.x = 0.f;
		spawn_pos.y += m_level_data.m_tile_size.y;
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

void LevelLoader::CreatePlatform(const EPlatformType type, const ETileType tile_type, LevelInfo& level_info, const int row, const int col, const SceneNode::Ptr& parent, const sf::Vector2f spawn_pos)
{
	std::unique_ptr<Platform> platform(new Platform(type, m_textures));
	AddPlatformParts(platform.get(), row, col, parent, tile_type, spawn_pos);

	if (tile_type == kImpactPlatform)
	{
		platform->SetType(EPlatformType::kImpact);
	}

	level_info.platforms.emplace_back(std::move(platform));
}

void LevelLoader::AddPlatformParts(Platform* platform, const int row, const int col, const SceneNode::Ptr& parent, const ETileType tile_type, sf::Vector2f spawn_pos)
{
	const ETileType type = static_cast<ETileType>(m_level_data_vector[row][col]);

	std::unique_ptr<PlatformPart> platform_part(m_tile_factory.CreatePlatformPart(type, spawn_pos, platform, tile_type));
	parent->AttachChild(std::move(platform_part));

	//Is vertical platform
	for(size_t i = row + 1; i < m_level_data_vector.size(); i++)
	{
		if(static_cast<ETileType>(m_level_data_vector[i][col]) != type)
			break;

		spawn_pos.y += m_level_data.m_tile_size.y;
		std::unique_ptr<PlatformPart> vertical_platform_part(m_tile_factory.CreatePlatformPart(type, spawn_pos, platform, tile_type));
		parent->AttachChild(std::move(vertical_platform_part));
		m_level_data_vector[i][col] = -1;
	}

	//Is horizontal platform
	for(size_t j = col + 1; j < m_level_data_vector[row].size(); j++)
	{
		if(static_cast<ETileType>(m_level_data_vector[row][j]) != type)
			break;

		spawn_pos.x += m_level_data.m_tile_size.x;
		std::unique_ptr<PlatformPart> horizontal_platform_part(m_tile_factory.CreatePlatformPart(type, spawn_pos, platform, tile_type));
		parent->AttachChild(std::move(horizontal_platform_part));
		m_level_data_vector[row][j] = -1;
	}
}