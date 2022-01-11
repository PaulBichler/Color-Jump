#pragma once
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "SceneNode.hpp"
#include "SpriteNode.hpp"
#include "Aircraft.hpp"
#include "Layers.hpp"
#include "AircraftType.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <array>

#include "Character.hpp"
#include "CommandQueue.hpp"
#include "LevelLoader.hpp"
#include "LevelManager.hpp"

//Foward
namespace sf
{
	class RenderWindow;
}


class World : private sf::NonCopyable
{
public:
	explicit World(sf::RenderWindow& window, FontHolder& font, LevelManager& level_manager);
	void Update(sf::Time dt);
	void Draw();
	CommandQueue& getCommandQueue();

private:
	void LoadTextures();
	void BuildScene();

	sf::FloatRect GetViewBounds() const;
	sf::FloatRect GetBattlefieldBounds() const;
	void SpawnEnemies();
	void AddEnemy(AircraftType type, float relX, float relY);
	void AddEnemies();
	void GuideMissiles();
	static void PlayerGroundRayCast(const std::set<SceneNode::Pair>& pairs);
	void GetGroundRayCasts(std::set<SceneNode::Pair>& pairs, const SceneNode::Pair pair, const Category::Type category) const;
	void HandleCollisions();
	void DestroyEntitiesOutsideView();

private:
	struct SpawnPoint
	{
		SpawnPoint(AircraftType type, float x, float y) : m_type(type), m_x(x), m_y(y)
		{
			
		}
		AircraftType m_type;
		float m_x;
		float m_y;
	};
	

private:
	sf::RenderWindow& m_window;
	sf::View m_camera;
	TextureHolder m_textures;
	FontHolder& m_fonts;
	SceneNode m_scenegraph;
	std::array<SceneNode*, static_cast<int>(Layers::kLayerCount)> m_scene_layers;
	CommandQueue m_command_queue;
	LevelLoader::LevelInfo m_level_info;
	LevelManager& m_level_manager;

	sf::FloatRect m_world_bounds;
	sf::Vector2f m_spawn_position;
	float m_scrollspeed;
	std::vector<SpawnPoint> m_enemy_spawn_points;
	std::vector<Aircraft*>	m_active_enemies;
};

