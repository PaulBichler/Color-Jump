#pragma once
#include <fstream>
#include <sstream>

#include "Character.hpp"
#include "LevelManager.hpp"
#include "ResourceIdentifiers.hpp"
#include "TileFactory.hpp"

struct LevelInfo
{
	SceneNode::Ptr level_parent;
	SceneNode::Ptr background_parent;
};

template<class T> 
class LevelLoader
{
public:
	LevelLoader(LevelManager::LevelData& level_data, TextureHolder& textures, SoundPlayer& sound_player);

	virtual T LoadLevel() = 0;

protected:
	virtual SceneNode::Ptr LoadLevelLayer(const std::string& csv_path, T& level_info, bool is_collider_layer) = 0;
	std::vector<std::vector<int>> LevelDataToVector(const std::string& csv_path) const;

protected:
	LevelManager::LevelData& m_level_data;
	TextureHolder& m_textures;
	TileFactory m_tile_factory;
	std::vector<std::vector<int>> m_level_data_vector;

};



//Written by Paul Bichler (D00242563)
//The LevelLoader class is used to construct levels based on the level data CSV files in the LevelManager.
//It uses the TileFactory class to create instances for the tile types specified in the level files.
template<class T>
LevelLoader<T>::LevelLoader(LevelManager::LevelData& level_data, TextureHolder& textures, SoundPlayer& sound_player)
	: m_level_data(level_data),
	m_textures(textures),
	m_tile_factory(textures, level_data.m_tile_size, sound_player)
{
}

//Written by Paul Bichler (D00242563)
//This method reads a csv level file line-by-line and converts it to a 2-dimensional vector
template<class T>
std::vector<std::vector<int>> LevelLoader<T>::LevelDataToVector(const std::string& csv_path) const
{
	std::vector<std::vector<int>> levelDataVector;
	std::ifstream in(csv_path);
	assert(!in.fail());

	std::string line;
	while (std::getline(in, line))
	{
		std::vector<int> levelLineData;

		std::stringstream ss(line);
		std::string token;

		while (std::getline(ss, token, ','))
			levelLineData.emplace_back(std::stoi(token));

		levelDataVector.emplace_back(levelLineData);
	}

	return levelDataVector;
}

