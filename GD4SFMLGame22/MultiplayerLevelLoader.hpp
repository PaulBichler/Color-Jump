#pragma once
#include "LevelLoader.hpp"

struct MLevelInfo : LevelInfo
{
	uint16_t m_number_of_tiles {};
	sf::IntRect m_blue_player_rect;
	sf::Vector2f m_blue_player_spawn_pos;
	sf::IntRect m_red_player_rect;
	sf::Vector2f m_red_player_spawn_pos;
};

class MultiplayerLevelLoader : LevelLoader<MLevelInfo>
{
public:
	MultiplayerLevelLoader(LevelManager::LevelData& level_data, TextureHolder& textures, SoundPlayer& sound_player);
	MLevelInfo LoadLevel() override;

protected:
	SceneNode::Ptr LoadLevelLayer(const std::string& csv_path, MLevelInfo& level_info, bool is_collider_layer) override;
};

