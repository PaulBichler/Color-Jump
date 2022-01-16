//Written by Paul Bichler (D00242563)

#include "LevelManager.hpp"

//Written by Paul Bichler (D00242563)
//This struct holds all the necessary level data info to load a level
LevelManager::LevelData::LevelData(std::string platform_layer_path, std::string background_layer_path, const sf::Vector2u tile_size)
	: m_platform_layer_path(std::move(platform_layer_path)),
	m_background_layer_path(std::move(background_layer_path)),
	m_tile_size(tile_size)
{
}

//Written by Paul Bichler (D00242563)
LevelManager::LevelManager() : m_current_index(0)
{
	//Holds level data info
	m_levels.emplace_back("LevelData/LevelData_Level1_Platforms.csv", "LevelData/LevelData_Level1_Background.csv", sf::Vector2u(64, 64));
	m_levels.emplace_back("LevelData/LevelData_Level2_Platforms.csv", "LevelData/LevelData_Level2_Background.csv", sf::Vector2u(64, 64));
	m_levels.emplace_back("LevelData/LevelData_Level3_Platforms.csv", "LevelData/LevelData_Level3_Background.csv", sf::Vector2u(64, 64));
	m_levels.emplace_back("LevelData/LevelData_Level4_Platforms.csv", "LevelData/LevelData_Level4_Background.csv", sf::Vector2u(64, 64));
	m_levels.emplace_back("LevelData/LevelData_Level5_Platforms.csv", "LevelData/LevelData_Level5_Background.csv", sf::Vector2u(64, 64));
	m_levels.emplace_back("LevelData/LevelData_Level6_Platforms.csv", "LevelData/LevelData_Level6_Background.csv", sf::Vector2u(64, 64));
	m_levels.emplace_back("LevelData/LevelData_Level7_Platforms.csv", "LevelData/LevelData_Level7_Background.csv", sf::Vector2u(64, 64));
	m_levels.emplace_back("LevelData/LevelData_Level8_Platforms.csv", "LevelData/LevelData_Level8_Background.csv", sf::Vector2u(64, 64));
	m_levels.emplace_back("LevelData/LevelData_Level9_Platforms.csv", "LevelData/LevelData_Level9_Background.csv", sf::Vector2u(64, 64));
}

//Written by Paul Bichler (D00242563)
LevelManager::LevelData LevelManager::GetCurrentLevelData() const
{
	return m_levels[m_current_index];
}

//Written by Paul Bichler (D00242563)
void LevelManager::NextLevel()
{
	if(DoesNextLevelExist())
		m_current_index++;
}

//Written by Paul Bichler (D00242563)
bool LevelManager::DoesNextLevelExist() const
{
	return m_current_index + 1 < m_levels.size();
}
