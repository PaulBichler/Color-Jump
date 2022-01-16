#pragma once
#include <SFML/Graphics/View.hpp>

#include "Character.hpp"
#include "LevelManager.hpp"
#include "ResourceIdentifiers.hpp"
#include "TileFactory.hpp"

class LevelLoader
{
public:
	LevelLoader(LevelManager::LevelData& level_data, TextureHolder& textures, SoundPlayer& sound_player);

	struct LevelInfo
	{
		Character* player_1{};
		Character* player_2{};
		SceneNode::Ptr level_parent;
		SceneNode::Ptr background_parent;
		std::vector<std::unique_ptr<Platform>> platforms;
	};
	
	LevelInfo LoadLevel();

private:
	SceneNode::Ptr LoadLevelLayer(LevelInfo& level_info, const std::string& csv_path, const bool is_collider_layer);
	std::vector<std::vector<int>> LevelDataToVector(const std::string& csv_path) const;

private:
	LevelManager::LevelData& m_level_data;
	TextureHolder& m_textures;
	TileFactory m_tile_factory;
	std::vector<std::vector<int>> m_level_data_vector;

	void CreatePlatform(const ETileType tile_type, LevelInfo& level_info, const int row, const int col, const SceneNode::Ptr& parent, const sf
	                    ::Vector2f spawn_pos);
	void AddPlatformParts(Platform* platform, int row, int col, const SceneNode::Ptr& parent, ETileType tile_type, sf::Vector2f spawn_pos);

};

