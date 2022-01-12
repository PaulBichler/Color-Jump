#include "World.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>
#include <limits>

#include "LevelLoader.hpp"
#include "Pickup.hpp"
#include "PlatformPart.hpp"
#include "Projectile.hpp"
#include "Utility.hpp"

World::World(sf::RenderWindow& window, FontHolder& font, LevelManager& level_manager)
	: m_window(window)
	  , m_camera(window.getDefaultView())
	  , m_fonts(font)
	  , m_scene_layers()
	  , m_level_manager(level_manager)
	  , m_world_bounds(0.f, 0.f, m_camera.getSize().x, m_camera.getSize().y)
	  , m_spawn_position(m_camera.getSize().x / 2.f, m_world_bounds.height - m_camera.getSize().y / 2.f)
	  , m_scrollspeed(0)
//, m_player_aircraft(nullptr)
{
	LoadTextures();
	BuildScene();
	Utility::Debug(std::to_string(m_camera.getSize().x) + " " + std::to_string(m_camera.getSize().y));
	m_camera.setCenter(m_spawn_position);
}

void World::Update(sf::Time dt)
{
	//Scroll the world
	m_camera.move(0, m_scrollspeed * dt.asSeconds());

	//m_player_aircraft->SetVelocity(0.f, 0.f);
	DestroyEntitiesOutsideView();

	//Forward commands to the scenegraph until the command queue is empty
	while (!m_command_queue.IsEmpty())
	{
		m_scenegraph.OnCommand(m_command_queue.Pop(), dt);
	}

	HandleCollisions();
	//Remove all destroyed entities
	m_scenegraph.RemoveWrecks();

	//Apply movement
	m_scenegraph.Update(dt, m_command_queue);
	/*AdaptPlayerPosition();*/
}

void World::Draw()
{
	m_window.setView(m_camera);
	m_window.draw(m_scenegraph);
}

void World::LoadTextures()
{
	m_textures.Load(Textures::kEagle, "Media/Textures/Eagle.png");
	m_textures.Load(Textures::kRaptor, "Media/Textures/Raptor.png");
	m_textures.Load(Textures::kAvenger, "Media/Textures/Avenger.png");
	m_textures.Load(Textures::kBackground, "Media/Textures/Desert.png");

	m_textures.Load(Textures::kBullet, "Media/Textures/Bullet.png");
	m_textures.Load(Textures::kMissile, "Media/Textures/Missile.png");

	m_textures.Load(Textures::kLevelTileSet, "Media/Textures/TileSet.png");
	m_textures.Load(Textures::kImpactPlatform, "Media/Textures/ImpactPlatform.png");
	m_textures.Load(Textures::kImpactRedPlatform, "Media/Textures/RedImpactPlatform.png");
	m_textures.Load(Textures::kImpactBluePlatform, "Media/Textures/BlueImpactPlatform.png");
}

void World::BuildScene()
{
	//Initialize the different layers
	for (std::size_t i = 0; i < static_cast<int>(Layers::kLayerCount); ++i)
	{
		Category::Type category = i == static_cast<int>(Layers::kLevel)
			                          ? Category::Type::kScene
			                          : Category::Type::kNone;
		SceneNode::Ptr layer(new SceneNode(category));
		m_scene_layers[i] = layer.get();
		m_scenegraph.AttachChild(std::move(layer));
	}

	LevelManager::LevelData current_level_data = m_level_manager.GetCurrentLevelData();
	LevelLoader level_loader(current_level_data, m_textures);

	m_level_info = level_loader.LoadLevel();
	m_scene_layers[static_cast<int>(Layers::kBackground)]->AttachChild(std::move(m_level_info.background_parent));
	m_scene_layers[static_cast<int>(Layers::kLevel)]->AttachChild(std::move(m_level_info.level_parent));
}

CommandQueue& World::getCommandQueue()
{
	return m_command_queue;
}

void World::SetLoseCallback(const std::function<void()>& callback)
{
	m_lose_callback = callback;
}

void World::SetWinCallback(const std::function<void()>& callback)
{
	m_win_callback = callback;
}

sf::FloatRect World::GetViewBounds() const
{
	return sf::FloatRect(m_camera.getCenter() - m_camera.getSize() / 2.f, m_camera.getSize());
}

sf::FloatRect World::GetBattlefieldBounds() const
{
	//Return camera bounds + a small area at the top where enemies spawn offscreen
	sf::FloatRect bounds = GetViewBounds();
	bounds.top -= 300.f;
	bounds.height += 300.f;

	return bounds;
}

bool MatchesCategories(SceneNode::Pair& collision, Category::Type type1, Category::Type type2)
{
	const unsigned int category1 = collision.first->GetCategory();
	const unsigned int category2 = collision.second->GetCategory();

	if (type1 & category1 && type2 & category2)
	{
		return true;
	}

	if (type1 & category2 && type2 & category1)
	{
		std::swap(collision.first, collision.second);
		return true;
	}

	return false;
}

void World::PlayerGroundRayCast(const std::set<SceneNode::Pair>& pairs)
{
	bool collide = false;
	SceneNode::Pair player_pair;

	for (SceneNode::Pair pair : pairs)
	{
		player_pair = pair;
		if (MatchesCategories(pair, Category::Type::kRay, Category::Type::kPlatform))
		{
			collide = true;
			break;
		}
	}

	if (player_pair.first == nullptr || player_pair.second == nullptr)
	{
		return;
	}

	if (!collide)
	{
		if (player_pair.first != nullptr && (player_pair.first->GetCategory() & Category::Type::kRay) != 0)
		{
			const auto& ray_ground = static_cast<RayGround&>(*player_pair.first);
			ray_ground.SetFalling();
		}
		else if (player_pair.second != nullptr && (player_pair.second->GetCategory() & Category::Type::kRay) != 0)
		{
			const auto& ray_ground = static_cast<RayGround&>(*player_pair.second);
			ray_ground.SetFalling();
		}
	}
}

void World::GetGroundRayCasts(std::set<SceneNode::Pair>& pairs, const SceneNode::Pair pair,
                              const Category::Type category) const
{
	if (pair.first->GetCategory() == category || pair.second->GetCategory() == category)
	{
		pairs.insert(std::minmax(pair.first, pair.second));
	}
}

void World::HandleCollisions()
{
	std::set<SceneNode::Pair> collision_pairs;
	m_scenegraph.CheckSceneCollision(m_scenegraph, collision_pairs);

	std::set<SceneNode::Pair> pairs_player_one;
	std::set<SceneNode::Pair> pairs_player_two;
	for (SceneNode::Pair pair : collision_pairs)
	{
		if (MatchesCategories(pair, Category::Type::kPlayer, Category::Type::kPlatform))
		{
			auto& player = static_cast<Character&>(*pair.first);
			auto& platform_part = static_cast<PlatformPart&>(*pair.second);
			Platform* platform = platform_part.GetPlatform();

			if (platform->DoesPlayerCollide(player.GetCharacterType()))
			{
				//Collision
				player.SetGrounded(platform);
			}

			//Check Win Condition
			if (!m_has_won && platform->GetPlatformType() == EPlatformType::kGoal)
			{
				if (m_level_info.player_1->IsOnPlatformOfType(EPlatformType::kGoal) &&
					m_level_info.player_2->IsOnPlatformOfType(EPlatformType::kGoal))
				{
					//Win
					m_win_callback();
					m_has_won = true;
				}
			}
		}

		GetGroundRayCasts(pairs_player_one, pair, Category::kRayOne);
		GetGroundRayCasts(pairs_player_two, pair, Category::kRayTwo);
	}

	PlayerGroundRayCast(pairs_player_one);
	PlayerGroundRayCast(pairs_player_two);
}

void World::DestroyEntitiesOutsideView()
{
	Command command;
	command.category = Category::Type::kPlayerOne | Category::kPlayerTwo;
	command.action = DerivedAction<Entity>([this](Entity& e, sf::Time)
	{
		//Does the object intersect with the battlefield
		if (!GetBattlefieldBounds().intersects(e.GetBoundingRect()))
		{
			e.Destroy();
			m_lose_callback();
		}
	});
	m_command_queue.Push(command);
}
