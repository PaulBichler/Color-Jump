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
		Character* player_1;
		Character* player_2;
		SceneNode::Ptr platforms_parent;
		SceneNode::Ptr background_parent;
	};

	LevelInfo LoadLevel(const LevelManager::LevelData& level_data) const;

private:
	SceneNode::Ptr LoadLevelLayer(LevelInfo& level_info, const std::string& csv_path, sf::Vector2u tile_size) const;

private:
	TextureHolder& m_textures;

};

