#pragma once
#include <SFML/Graphics/View.hpp>

#include "LevelManager.hpp"
#include "ResourceIdentifiers.hpp"
#include "TileFactory.hpp"

class LevelLoader
{
private:
	TextureHolder& m_textures;

public:
	LevelLoader(TextureHolder& textures);

	SceneNode::Ptr LoadLevel(const std::string& csv_path, sf::Vector2u tile_size) const;
};

