#include "LevelManager.hpp"

LevelManager::LevelData::LevelData(std::string platform_layer_path, std::string background_layer_path, sf::Vector2u tile_size)
	: m_platform_layer_path(std::move(platform_layer_path)),
	m_background_layer_path(std::move(background_layer_path)),
	m_tile_size(tile_size)
{
}

LevelManager::LevelManager() : m_current_index(0)
{
	//Holds level data file path
	m_levels.emplace_back("LevelData/level_1_Platforms.csv", "LevelData/level_1_Background.csv", sf::Vector2u(64, 64));
}

LevelManager::LevelData LevelManager::GetCurrentLevelData() const
{
	return m_levels[m_current_index];
}
