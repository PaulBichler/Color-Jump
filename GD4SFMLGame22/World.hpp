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
	void Update(sf::Time dt);
	void Draw();
	CommandQueue& getCommandQueue();

	void SetLoseCallback(const std::function<void()>& callback);
	void SetWinCallback(const std::function<void()>& callback);

private:
	void LoadTextures();
	void BuildScene();

	sf::FloatRect GetViewBounds() const;
	sf::FloatRect GetBattlefieldBounds() const;
	static bool IsPlayerBelowPlatform(const Character& player, const PlatformPart& platform_part);
	static bool CheckPlatform(const Platform* platform, ECharacterType character);
	static bool IsPlayerAtHisPlatform(const Character& player, const Platform* platform);
	void HandleCollisions();
	void DestroyEntitiesOutsideView();
	void UpdateSounds() const;
	void UpdatePlatforms(sf::Time dt) const;

	static void PlayerGroundRayCast(const std::set<SceneNode::Pair>& pairs);
	void GetGroundRayCasts(std::set<SceneNode::Pair>& pairs, SceneNode::Pair pair, Category::Type category) const;


private:
	sf::RenderTarget& m_target;
	sf::RenderTexture m_scene_texture;
	sf::View m_camera;
	TextureHolder m_textures;
	SoundPlayer& m_sounds;
	SceneNode m_sceneGraph;
	std::array<SceneNode*, static_cast<int>(Layers::kLayerCount)> m_scene_layers;
	CommandQueue m_command_queue;

	sf::FloatRect m_world_bounds;
	sf::Vector2f m_spawn_position;
	float m_scrollSpeed;

	BloomEffect m_bloom_effect;

	LevelLoader::LevelInfo m_level_info;
	LevelManager& m_level_manager;
	std::function<void()> m_lose_callback;
	std::function<void()> m_win_callback;
	bool m_has_won{};
};
