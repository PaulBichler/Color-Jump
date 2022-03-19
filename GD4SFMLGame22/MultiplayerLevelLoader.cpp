#include "MultiplayerLevelLoader.hpp"

MultiplayerLevelLoader::MultiplayerLevelLoader(LevelManager::LevelData& level_data, TextureHolder& textures, SoundPlayer& sound_player)
	: LevelLoader<MLevelInfo>(level_data, textures, sound_player)
{
}

MLevelInfo MultiplayerLevelLoader::LoadLevel()
{
	MLevelInfo level_info;
	level_info.background_parent = LoadLevelLayer(m_level_data.m_background_layer_path, level_info, false);
	level_info.level_parent = LoadLevelLayer(m_level_data.m_platform_layer_path, level_info, true);

	return level_info;
}

SceneNode::Ptr MultiplayerLevelLoader::LoadLevelLayer(const std::string& csv_path, MLevelInfo& level_info, bool is_collider_layer)
{
	//Create the root node
	SceneNode::Ptr level_parent = std::make_unique<SceneNode>();
	sf::Vector2f spawn_pos(0.f, 0.f);

	//Convert the csv level data into a 2-dimensional vector (this makes it easier to create platforms)
	m_level_data_vector = LevelDataToVector(csv_path);

	//Loop through the 2-dimensional level vector and construct the level tile by tile
	for (int row = 0; row < m_level_data_vector.size(); row++)
	{
		for (int col = 0; col < m_level_data_vector[row].size(); col++)
		{
			//The id in the level data CSV is converted to a TileType enum to make the code more readable and easily adjustable
			int id = m_level_data_vector[row][col];
			ETileType tile_type = static_cast<ETileType>(id);

			//Construct the tile based on the tile type
			switch (tile_type)
			{
			case kHorizontalImpactPlatformPart:
			case kVerticalImpactPlatformPart:
			{
				SceneNode::Ptr colorTile(m_tile_factory.CreateColorTile(tile_type, EColorType::kNone, spawn_pos));
				level_info.number_of_tiles++;
				level_parent->AttachChild(std::move(colorTile));
			}
			break;
			case kNone:
				//Tiles marked with (-1 = kNone) do nothing
				break;
			default:
			{
				//Tiles that don't correspond to one of the types above have no special functionality,
				//which is why they are created here with the base "Tile" class.
				SceneNode::Ptr tilePtr(m_tile_factory.CreateTile(tile_type, spawn_pos, is_collider_layer));
				if (tilePtr.get() != nullptr)
					level_parent->AttachChild(std::move(tilePtr));
			}
			break;
			}

			spawn_pos.x += m_level_data.m_tile_size.x;
		}

		spawn_pos.x = 0.f;
		spawn_pos.y += m_level_data.m_tile_size.y;
	}

	return level_parent;
}
