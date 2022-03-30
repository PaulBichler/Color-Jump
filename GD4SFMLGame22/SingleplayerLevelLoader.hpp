#pragma once
#include "LevelLoader.hpp"

struct SLevelInfo : LevelInfo
{
	std::vector<std::unique_ptr<Platform>> platforms;
};

class SingleplayerLevelLoader : public LevelLoader<SLevelInfo>
{
public:
	SingleplayerLevelLoader(LevelManager::LevelData& level_data, TextureHolder& textures, SoundPlayer& sound_player);
	SLevelInfo LoadLevel() override;

private:
	SceneNode::Ptr LoadLevelLayer(const std::string& csv_path, SLevelInfo& level_info, bool is_collider_layer) override;

	void CreatePlatform(SLevelInfo& level_info, ETileType tile_type, int row, int col, const SceneNode::Ptr& parent, sf::Vector2f spawn_pos);
	void AddPlatformParts(Platform* platform, int row, int col, const SceneNode::Ptr& parent, ETileType tile_type, sf::Vector2f spawn_pos);
};

