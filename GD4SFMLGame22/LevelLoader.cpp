#include "LevelLoader.hpp"

#include <fstream>
#include <sstream>
#include <SFML/Graphics/View.hpp>

#include "ETileType.hpp"
#include "ResourceHolder.hpp"

LevelLoader::LevelLoader(TextureHolder& textures)
	: m_textures(textures)
{
}

LevelLoader::LevelInfo LevelLoader::LoadLevel(const LevelManager::LevelData& level_data) const
{
	LevelInfo level_info;
	level_info.background_parent = LoadLevelLayer(level_info, level_data.m_background_layer_path, level_data.m_tile_size);
	level_info.platforms_parent = LoadLevelLayer(level_info, level_data.m_platform_layer_path, level_data.m_tile_size);

	return level_info;
}

SceneNode::Ptr LevelLoader::LoadLevelLayer(LevelInfo& level_info, const std::string& csv_path, sf::Vector2u tile_size) const
{
	SceneNode::Ptr levelParent = std::make_unique<SceneNode>();

	const TileFactory tile_factory(m_textures, tile_size);

	std::ifstream in(csv_path);
	assert(!in.fail());

	/*sf::Vector2f spawn_pos(tile_size.x / 2.f, tile_size.y / 2.f);*/
	sf::Vector2f spawn_pos(0.f, 0.f);

	std::string line;
	while (std::getline(in, line))
	{
		std::stringstream ss(line);
		std::string token;
		while (std::getline(ss, token, ','))
		{
			int id = std::stoi(token);
			ETileType tile_type = static_cast<ETileType>(id);

			if(tile_type == kRedPlayer)
			{
				std::unique_ptr<Character> player_1(tile_factory.CreatePlayer(id, ECharacterType::kRed));
				levelParent->AttachChild(std::move(player_1));
				level_info.player_1 = player_1.get();
			}
			else if(tile_type == kBluePlayer)
			{
				std::unique_ptr<Character> player_2(tile_factory.CreatePlayer(id, ECharacterType::kBlue));
				levelParent->AttachChild(std::move(player_2));
				level_info.player_2 = player_2.get();
			}
			else if(tile_type != kNone)
			{
				SceneNode::Ptr tilePtr(tile_factory.CreateTile(id, spawn_pos));
				if(tilePtr.get() != nullptr)
					levelParent->AttachChild(std::move(tilePtr));
			}


			spawn_pos.x += tile_size.x;
		}

		/*spawn_pos.x = tile_size.x / 2.f;*/
		spawn_pos.x = 0.f;
		spawn_pos.y += tile_size.y;
	}

	return levelParent;
}
