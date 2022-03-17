//Written by Paul Bichler (D00242563)

#include "LevelLoader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include "ResourceHolder.hpp"

////Written by Paul Bichler (D00242563)
////The LevelLoader class is used to construct levels based on the level data CSV files in the LevelManager.
////It uses the TileFactory class to create instances for the tile types specified in the level files.
//LevelLoader::LevelLoader(LevelManager::LevelData& level_data, TextureHolder& textures, SoundPlayer& sound_player)
//	: m_level_data(level_data),
//	  m_textures(textures),
//	  m_tile_factory(textures, level_data.m_tile_size, sound_player)
//{
//}
//
////Written by Paul Bichler (D00242563)
////This method reads a csv level file line-by-line and converts it to a 2-dimensional vector
//std::vector<std::vector<int>> LevelLoader::LevelDataToVector(const std::string& csv_path) const
//{
//	std::vector<std::vector<int>> levelDataVector;
//	std::ifstream in(csv_path);
//	assert(!in.fail());
//
//	std::string line;
//	while (std::getline(in, line))
//	{
//		std::vector<int> levelLineData;
//
//		std::stringstream ss(line);
//		std::string token;
//
//		while (std::getline(ss, token, ','))
//			levelLineData.emplace_back(std::stoi(token));
//
//		levelDataVector.emplace_back(levelLineData);
//	}
//
//	return levelDataVector;
//}
