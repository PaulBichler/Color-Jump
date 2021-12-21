#include "LevelLoader.hpp"

#include <fstream>
#include <sstream>
#include <SFML/Graphics/View.hpp>

#include "ResourceHolder.hpp"

LevelLoader::LevelLoader(TextureHolder& textures)
	: m_textures(textures)
{
}

SceneNode::Ptr LevelLoader::LoadLevel(const std::string& csv_path, sf::Vector2u tile_size) const
{
	SceneNode::Ptr levelParent = std::make_unique<SceneNode>();

	const TileFactory tile_factory(m_textures, tile_size);

	std::ifstream in(csv_path);
	assert(!in.fail());

	/*sf::Vector2f spawn_pos(tile_size.x / 2.f, tile_size.y / 2.f);*/
	sf::Vector2f spawn_pos(0.f, 0.f);

	std::string line;
	while (std::getline(in, line))
	{
		std::stringstream ss(line);
		std::string token;
		while (std::getline(ss, token, ','))
		{
			Tile* tilePtr = tile_factory.CreateTile(std::stoi(token), spawn_pos);

			if(tilePtr != nullptr)
			{
				std::unique_ptr<Tile> tile(tilePtr);
				levelParent->AttachChild(std::move(tile));
			}

			spawn_pos.x += tile_size.x;
		}

		/*spawn_pos.x = tile_size.x / 2.f;*/
		spawn_pos.x = 0.f;
		spawn_pos.y += tile_size.y;
	}

	return levelParent;
}
