#pragma once
#include <SFML/Graphics/View.hpp>

#include "Character.hpp"
#include "LevelManager.hpp"
#include "ResourceIdentifiers.hpp"
#include "TileFactory.hpp"

class LevelLoader
{
public:
	LevelLoader(TextureHolder& textures);

	struct LevelInfo
	{
		Character* player_1{};
		Character* player_2{};
		SceneNode::Ptr level_parent;
		SceneNode::Ptr background_parent;
		std::vector<std::unique_ptr<Platform>> platforms;
		sf::Vector2u tile_size;
	};

	LevelInfo LoadLevel(const LevelManager::LevelData& level_data) const;

private:
	SceneNode::Ptr LoadLevelLayer(LevelInfo& level_info, const std::string& csv_path) const;
	std::vector<std::vector<int>> LevelDataToVector(const std::string& csv_path) const;

private:
	TextureHolder& m_textures;

	void CreatePlatform(LevelInfo& level_info, std::vector<std::vector<int>>& level_data, int row, int col, SceneNode::Ptr& parent, TileFactory& tile_factory, sf::Vector2f spawn_pos) const;

};

