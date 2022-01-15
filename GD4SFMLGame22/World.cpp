#include "World.hpp"

#include <iostream>

#include "ParticleNode.hpp"
#include "PostEffect.hpp"
#include "Utility.hpp"

#include "PlatformPart.hpp"


World::World(sf::RenderTarget& output_target, SoundPlayer& sounds, LevelManager& level_manager)
	: m_target(output_target)
	  , m_camera(output_target.getDefaultView())
	  , m_sounds(sounds)
	  , m_scene_layers()
	  , m_world_bounds(0.f, 0.f, m_camera.getSize().x, m_camera.getSize().y)
	  , m_spawn_position(m_camera.getSize().x / 2.f, m_world_bounds.height - m_camera.getSize().y / 2.f)
	  , m_scrollSpeed(0)
	  , m_level_manager(level_manager)
{
	m_scene_texture.create(m_target.getSize().x, m_target.getSize().y);

	LoadTextures();
	BuildScene();
	m_camera.setCenter(m_spawn_position);
}

void World::Update(const sf::Time dt)
{
	UpdateSounds();
	UpdatePlatforms(dt);

	//Scroll the world
	m_camera.move(0, m_scrollSpeed * dt.asSeconds());

	DestroyEntitiesOutsideView();

	//Forward commands to the sceneGraph until the command queue is empty
	while (!m_command_queue.IsEmpty())
	{
		m_sceneGraph.OnCommand(m_command_queue.Pop(), dt);
	}

	HandleCollisions();
	//Remove all destroyed entities
	m_sceneGraph.RemoveWrecks();


	//Apply movement
	m_sceneGraph.Update(dt, m_command_queue);
}

void World::Draw()
{
	if (PostEffect::IsSupported())
	{
		m_scene_texture.clear(sf::Color(0, 37, 97));
		m_scene_texture.setView(m_camera);
		m_scene_texture.draw(m_sceneGraph);
		m_scene_texture.display();
		m_bloom_effect.Apply(m_scene_texture, m_target);
	}
	else
	{
		m_target.setView(m_camera);
		m_target.draw(m_sceneGraph);
	}
}

void World::LoadTextures()
{
	m_textures.Load(Textures::kParticle, "Media/Textures/Particle.png");

	m_textures.Load(Textures::kLevelTileSet, "Media/Textures/spritesheet.png");
	m_textures.Load(Textures::kHImpactRedPlatform, "Media/Textures/RedImpactPlatform.png");
	m_textures.Load(Textures::kHImpactBluePlatform, "Media/Textures/BlueImpactPlatform.png");
	m_textures.Load(Textures::kVImpactRedPlatform, "Media/Textures/VRedImpactPlatform.png");
	m_textures.Load(Textures::kVImpactBluePlatform, "Media/Textures/VBlueImpactPlatform.png");
	m_textures.Load(Textures::kJumpSmoke, "Media/Textures/Explosion.png");
}

void World::BuildScene()
{
	//Initialize the different layers
	for (std::size_t i = 0; i < static_cast<int>(Layers::kLayerCount); ++i)
	{
		Category::Type category;
		if (i == static_cast<int>(Layers::kLevel))
			category = Category::Type::kScene;
		else
			category = Category::Type::kNone;
		SceneNode::Ptr layer(new SceneNode(category));
		m_scene_layers[i] = layer.get();
		m_sceneGraph.AttachChild(std::move(layer));
	}

	LevelManager::LevelData current_level_data = m_level_manager.GetCurrentLevelData();
	LevelLoader level_loader(current_level_data, m_textures, m_sounds);

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
	const auto view_bounds = sf::FloatRect(m_camera.getCenter() - m_camera.getSize() / 2.f, m_camera.getSize());
	return view_bounds;
}

sf::FloatRect World::GetBattlefieldBounds() const
{
	//Return camera bounds + a small area at the top where enemies spawn offscreen
	sf::FloatRect bounds = GetViewBounds();
	bounds.top -= 300.f;
	bounds.height += 300.f;

	return bounds;
}

bool World::IsPlayerBelowPlatform(const Character& player, const PlatformPart& platform_part)
{
	if (player.getPosition().y > platform_part.getPosition().y)
	{
		return true;
	}

	return false;
}

bool World::CheckPlatform(const Platform* platform, const ECharacterType character)
{
	if (character == ECharacterType::kBlue)
	{
		if (platform->GetPlatformType() == EPlatformType::kHorizontalBlue || platform->GetPlatformType() ==
			EPlatformType::kVerticalBlue)
		{
			return true;
		}
	}
	else if (character == ECharacterType::kRed)
	{
		if (platform->GetPlatformType() == EPlatformType::kHorizontalRed || platform->GetPlatformType() ==
			EPlatformType::kVerticalRed)
		{
			return true;
		}
	}

	return false;
}

bool World::IsPlayerAtHisPlatform(const Character& player, const Platform* platform)
{
	if (platform->GetPlatformType() == EPlatformType::kNormal || platform->GetPlatformType() == EPlatformType::kGoal)
	{
		return true;
	}

	if (platform->GetPlatformType() == EPlatformType::kHorizontalImpact
		|| platform->GetPlatformType() == EPlatformType::kVerticalImpact)
	{
		return true;
	}


	if (player.GetCharacterType() == ECharacterType::kBlue)
	{
		return CheckPlatform(platform, ECharacterType::kBlue);
	}

	if (player.GetCharacterType() == ECharacterType::kRed)
	{
		return CheckPlatform(platform, ECharacterType::kRed);
	}

	return false;
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
			const auto& ray_ground = dynamic_cast<RayGround&>(*pair.first);
			auto& platform_part = dynamic_cast<PlatformPart&>(*pair.second);
			const Platform* platform = platform_part.GetPlatform();
			const Character* player = ray_ground.m_character;

			if (CheckPlatformUnderneath(player->GetCharacterType(), platform->GetPlatformType()))
			{
				collide = true;
				break;
			}
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
			const auto& ray_ground = dynamic_cast<RayGround&>(*player_pair.first);
			ray_ground.SetFalling();
		}
		else if (player_pair.second != nullptr && (player_pair.second->GetCategory() & Category::Type::kRay) != 0)
		{
			const auto& ray_ground = dynamic_cast<RayGround&>(*player_pair.second);
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
	m_sceneGraph.CheckSceneCollision(m_sceneGraph, collision_pairs);

	std::set<SceneNode::Pair> pairs_player_one;
	std::set<SceneNode::Pair> pairs_player_two;
	for (SceneNode::Pair pair : collision_pairs)
	{
		if (MatchesCategories(pair, Category::Type::kPlayer, Category::Type::kPlatform))
		{
			auto& player = dynamic_cast<Character&>(*pair.first);
			auto& platform_part = dynamic_cast<PlatformPart&>(*pair.second);
			Platform* platform = platform_part.GetPlatform();


			if (IsPlayerBelowPlatform(player, platform_part))
			{
				if (IsPlayerAtHisPlatform(player, platform))
				{
					player.MoveOutOfCollision(platform_part.GetBoundingRect());
					player.StopMovement();

					if (platform->GetPlatformType() == EPlatformType::kVerticalImpact)
					{
						platform->DoesPlayerCollide(player.GetCharacterType());
					}
					return;
				}
				return;
			}

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

		if (MatchesCategories(pair, Category::Type::kPlayer, Category::Type::kEnemyTrap))
		{
			m_lose_callback();
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
	command.category = Category::Type::kPlayerOne | Category::Type::kPlayerTwo;
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

void World::UpdateSounds() const
{
	// Set listener's position to player position
	m_sounds.SetListenerPosition(m_level_info.player_1->GetWorldPosition());

	// Remove unused sounds
	m_sounds.RemoveStoppedSounds();
}

void World::UpdatePlatforms(const sf::Time dt) const
{
	for (const auto& platform : m_level_info.platforms)
		platform->Update(dt);
}

bool World::CheckPlatformUnderneath(const ECharacterType character, const EPlatformType platform)
{
	if (platform == EPlatformType::kGoal || platform == EPlatformType::kNormal)
	{
		return true;
	}

	if (character == ECharacterType::kRed)
	{
		if (platform == EPlatformType::kVerticalRed || platform == EPlatformType::kHorizontalRed)
		{
			return true;
		}
	}

	if (character == ECharacterType::kBlue)
	{
		if (platform == EPlatformType::kVerticalBlue || platform == EPlatformType::kHorizontalBlue)
		{
			return true;
		}
	}

	return false;
}
