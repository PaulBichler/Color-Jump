#pragma once
#include <string>
#include <vector>
#include <SFML/System/Vector2.hpp>

class LevelManager
{
public:
	struct LevelData
	{
		LevelData(std::string platform_layer_path, std::string background_layer_path, sf::Vector2u tile_size);

		std::string m_platform_layer_path;
		std::string m_background_layer_path;
		sf::Vector2u m_tile_size;
	};

public:
	LevelManager();
	LevelData GetCurrentLevelData() const;
	void NextLevel();
	bool DoesNextLevelExist() const;
	void SetIsMultiplayer(bool is_multiplayer);

private:
	std::vector<LevelData> m_coop_levels;
	std::vector<LevelData> m_multiplayer_levels;
	int m_current_coop_index;
	int m_current_multiplayer_index;
	bool m_is_multiplayer{};
};

