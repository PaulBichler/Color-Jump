#pragma once
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "SceneNode.hpp"
#include "Layers.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

#include <array>

#include "BloomEffect.hpp"
#include "CommandQueue.hpp"
#include "SoundPlayer.hpp"

#include "LevelLoader.hpp"
#include "LevelManager.hpp"

class World : sf::NonCopyable
{
public:
	World(sf::RenderTarget& output_target, SoundPlayer& sounds, LevelManager& level_manager);
	void BuildWorld(LevelManager::LevelData current_level_data);
	virtual void Update(sf::Time dt);
	virtual void Draw();
	CommandQueue& GetCommandQueue();

private:
	void LoadTextures();
	void InitializeSceneLayers();
	virtual LevelInfo& BuildLevel(LevelManager::LevelData current_level_data) = 0;

protected:
	sf::FloatRect GetViewBounds() const;
	static bool IsPlayerBelowPlatform(const Character& player, const PlatformPart& platform_part);
	static bool CheckPlatform(const Platform* platform, EColorType character);
	static bool IsPlayerAtHisPlatform(const Character& player, const Platform* platform);
	static bool CheckPlatformUnderneath(EColorType character, EPlatformType platform);
	static void PlayerGroundRayCast(const std::set<SceneNode::Pair>& pairs);
	void GetGroundRayCasts(std::set<SceneNode::Pair>& pairs, SceneNode::Pair pair, Category::Type category) const;
	static bool MatchesCategories(SceneNode::Pair& collision, Category::Type type1, Category::Type type2);

	virtual void HandleCollisions() = 0;
	virtual sf::FloatRect GetBattlefieldBounds() const = 0;
	virtual void SetCamera() = 0;


protected:
	sf::RenderTarget& m_target;
	sf::RenderTexture m_scene_texture;
	sf::View m_camera;
	TextureHolder m_textures;
	SoundPlayer& m_sounds;
	SceneNode m_sceneGraph;
	std::array<SceneNode*, static_cast<int>(Layers::kLayerCount)> m_scene_layers;
	CommandQueue m_command_queue;

	sf::FloatRect m_world_bounds;
	BloomEffect m_bloom_effect;
};
