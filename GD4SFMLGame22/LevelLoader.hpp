#pragma once
#include "Character.hpp"
#include "LevelManager.hpp"
#include "ResourceIdentifiers.hpp"
#include "TileFactory.hpp"

struct LevelInfo
{
	SceneNode::Ptr level_parent;
	SceneNode::Ptr background_parent;

	sf::IntRect m_blue_player_rect;
	sf::Vector2f m_blue_player_spawn_pos;
	sf::IntRect m_red_player_rect;
	sf::Vector2f m_red_player_spawn_pos;

	std::vector<std::unique_ptr<Platform>> platforms;

	Platform* GetPlatformWithID(const sf::Int8 platform_id) const
	{
		for (const auto& platform : platforms)
			if (platform->GetID() == platform_id)
				return platform.get();

		return nullptr;
	}
};

class LevelLoader
{
public:
	virtual ~LevelLoader() = default;
	LevelLoader(LevelManager::LevelData& level_data, TextureHolder& textures);
	LevelInfo LoadLevel();

private:
	virtual SceneNode::Ptr LoadLevelLayer(const std::string& csv_path, LevelInfo& level_info,
	                                      bool is_collider_layer);
	std::vector<std::vector<int>> LevelDataToVector(const std::string& csv_path) const;
	void CreatePlatform(LevelInfo& level_info, ETileType tile_type, int row, int col,
	                    SceneNode::Ptr& parent, sf::Vector2f spawn_pos, sf::Int8 platform_id);
	void AddPlatformParts(Platform* platform, int row, int col, SceneNode::Ptr& parent,
	                      ETileType tile_type, sf::Vector2f spawn_pos);

private:
	LevelManager::LevelData& m_level_data;
	TextureHolder& m_textures;
	TileFactory m_tile_factory;
	std::vector<std::vector<int>> m_level_data_vector;
};
