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
	virtual ~World() = default;
	World(sf::RenderTarget& output_target, SoundPlayer& sounds);

	void BuildWorld(LevelManager::LevelData current_level_data);
	virtual void Update(sf::Time dt);
	virtual void Draw();
	CommandQueue& GetCommandQueue();

	Character* AddCharacter(sf::Int32 identifier);

private:
	void LoadTextures();
	void InitializeSceneLayers();
	virtual void LoadLevel(LevelManager::LevelData current_level_data);
	Character* AddCharacterWithColor(sf::Int32 identifier, EColorType color, sf::IntRect rect, sf::Vector2f spawn_pos);

protected:
	sf::FloatRect GetViewBounds() const;

	virtual void HandleCollisions() = 0;
	virtual sf::FloatRect GetBattlefieldBounds() const = 0;
	virtual void SetCamera() = 0;
	
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

	LevelInfo m_level_info;
	std::vector<Character*> m_players;
};
