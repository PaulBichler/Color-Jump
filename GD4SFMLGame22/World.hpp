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
	virtual void Update(sf::Time dt);
	virtual void Draw();
	CommandQueue& getCommandQueue();

private:
	void LoadTextures();
	void BuildScene();

protected:
	sf::FloatRect GetViewBounds() const;
	static bool IsPlayerBelowPlatform(const Character& player, const PlatformPart& platform_part);
	static bool CheckPlatform(const Platform* platform, ECharacterType character);
	static bool IsPlayerAtHisPlatform(const Character& player, const Platform* platform);
	void UpdateSounds() const;
	void UpdatePlatforms(sf::Time dt) const;
	static bool CheckPlatformUnderneath(ECharacterType character, EPlatformType platform);
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

	LevelLoader::LevelInfo m_level_info;
	LevelManager& m_level_manager;
};
