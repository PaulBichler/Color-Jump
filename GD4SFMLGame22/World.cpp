//Written by Paul Bichler (D00242563) and Dylan Goncalves Martins (D00242562)

#include "World.hpp"

#include <iostream>
#include <utility>

#include "ParticleNode.hpp"
#include "PostEffect.hpp"
#include "Utility.hpp"

#include "PlatformPart.hpp"


World::World(sf::RenderTarget& output_target, SoundPlayer& sounds)
	: m_target(output_target)
	  , m_camera(output_target.getDefaultView())
	  , m_sounds(sounds)
	  , m_scene_layers()
	  , m_world_bounds(0.f, 0.f, m_camera.getSize().x, m_camera.getSize().y)
{
	m_scene_texture.create(m_target.getSize().x, m_target.getSize().y);

	LoadTextures();
	InitializeSceneLayers();
	m_camera.setCenter(m_camera.getSize().x / 2.f, m_world_bounds.height - m_camera.getSize().y / 2.f);
}

void World::Update(const sf::Time dt)
{
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
	m_textures.Load(Textures::kLevelTileSet, "Media/Textures/spritesheet.png");
	m_textures.Load(Textures::kHImpactBlankPlatform, "Media/Textures/BlankPlatform.png");
	m_textures.Load(Textures::kHImpactRedPlatform, "Media/Textures/RedImpactPlatform.png");
	m_textures.Load(Textures::kHImpactBluePlatform, "Media/Textures/BlueImpactPlatform.png");
	m_textures.Load(Textures::kHImpactGreenPlatform, "Media/Textures/GreenPlatform.png");

	m_textures.Load(Textures::kVImpactBlankPlatform, "Media/Textures/VBlankPlatform.png");
	m_textures.Load(Textures::kVImpactRedPlatform, "Media/Textures/VRedImpactPlatform.png");
	m_textures.Load(Textures::kVImpactBluePlatform, "Media/Textures/VBlueImpactPlatform.png");
	m_textures.Load(Textures::kVImpactGreenPlatform, "Media/Textures/VGreenPlatform.png");

	m_textures.Load(Textures::kParticle, "Media/Textures/Particle.png");
	m_textures.Load(Textures::kJumpSmoke, "Media/Textures/Explosion.png");
}

void World::InitializeSceneLayers()
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
}

//Written by Paul Bichler (D00242563)
void World::BuildWorld(LevelManager::LevelData current_level_data)
{
	//Load the level based on the level data in the level manager 
	LevelInfo& level_info = BuildLevel(std::move(current_level_data));

	m_scene_layers[static_cast<int>(Layers::kBackground)]->AttachChild(
		std::move(level_info.background_parent));
	m_scene_layers[static_cast<int>(Layers::kLevel)]->AttachChild(
		std::move(level_info.level_parent));
}

CommandQueue& World::GetCommandQueue()
{
	return m_command_queue;
}

sf::FloatRect World::GetViewBounds() const
{
	const auto view_bounds = sf::FloatRect(m_camera.getCenter() - m_camera.getSize() / 2.f,
	                                       m_camera.getSize());
	return view_bounds;
}

/*
 *	Dylan Goncalves Martins (D00242562)
 *	This method checks if player is above the center of the platform
 */
bool World::IsPlayerBelowPlatform(const Character& player, const PlatformPart& platform_part)
{
	if (player.getPosition().y > platform_part.getPosition().y)
	{
		return true;
	}

	return false;
}

bool World::IsPlayerAboveTile(const Character& player, const Tile& tile)
{
	if (player.getPosition().y > tile.getPosition().y)
	{
		return true;
	}

	return false;
}

/*
 *	Dylan Goncalves Martins (D00242562)
 *	Returns true if platform color matches character color
 */
bool World::CheckPlatform(const Platform* platform, const EColorType character)
{
	if (character == EColorType::kBlue)
	{
		if (platform->GetPlatformType() == EPlatformType::kHorizontalBlue || platform->
			GetPlatformType() ==
			EPlatformType::kVerticalBlue)
		{
			return true;
		}
	}
	else if (character == EColorType::kRed)
	{
		if (platform->GetPlatformType() == EPlatformType::kHorizontalRed || platform->
			GetPlatformType() ==
			EPlatformType::kVerticalRed)
		{
			return true;
		}
	}

	return false;
}

bool World::CheckTile(const Tile& tile, const EColorType character)
{
	if (character == EColorType::kBlue)
	{
		if (tile.GetType() == kHorizontalBluePlatformPart || tile.GetType() ==
			kVerticalBluePlatformPart)
		{
			return true;
		}
	}
	else if (character == EColorType::kRed)
	{
		if (tile.GetType() == kHorizontalRedPlatformPart || tile.GetType() ==
			kVerticalRedPlatformPart)
		{
			return true;
		}
	}

	return false;
}

/*
 *	Dylan Goncalves Martins (D00242562)
 *	returns true if it is the appropriate platform
 */
bool World::IsPlayerAtHisPlatform(const Character& player, const Platform* platform)
{
	if (platform->GetPlatformType() == EPlatformType::kNormal || platform->GetPlatformType() ==
		EPlatformType::kGoal)
	{
		return true;
	}

	if (platform->GetPlatformType() == EPlatformType::kHorizontalImpact
		|| platform->GetPlatformType() == EPlatformType::kVerticalImpact)
	{
		return true;
	}


	if (player.GetCharacterType() == EColorType::kBlue)
	{
		return CheckPlatform(platform, EColorType::kBlue);
	}

	if (player.GetCharacterType() == EColorType::kRed)
	{
		return CheckPlatform(platform, EColorType::kRed);
	}

	return false;
}

bool World::IsPlayerAtHisTile(const Character& player, const Tile& tile)
{
	if (tile.GetType() == kHorizontalPlatformPart || tile.GetType() == kFinishPlatformPart)
	{
		return true;
	}

	if (tile.GetType() == kHorizontalImpactPlatformPart || tile.GetType() ==
		kVerticalImpactPlatformPart)
	{
		return true;
	}


	if (player.GetCharacterType() == EColorType::kBlue)
	{
		return CheckTile(tile, EColorType::kBlue);
	}

	if (player.GetCharacterType() == EColorType::kRed)
	{
		return CheckTile(tile, EColorType::kRed);
	}

	return false;
}

bool World::MatchesCategories(SceneNode::Pair& collision, Category::Type type1,
                              Category::Type type2)
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

Character* World::AddCharacterWithColor(const sf::Int32 identifier,
	const EColorType color, const sf::IntRect rect,
	const sf::Vector2f spawn_pos)
{
	std::unique_ptr<Character> player(
		new Character(color, m_textures, rect, m_sounds));
	player->setPosition(spawn_pos);
	player->SetIdentifier(identifier);

	m_players.emplace_back(player.get());
	m_scene_layers[static_cast<int>(Layers::kCharacters)]->AttachChild(std::move(player));
	return m_players.back();
}


Character* World::AddCharacter(const sf::Int32 identifier)
{
	if (identifier % 2 == 0)
	{
		return AddCharacterWithColor(identifier, EColorType::kRed, m_level_info.m_red_player_rect,
			m_level_info.m_red_player_spawn_pos);
	}

	return AddCharacterWithColor(identifier, EColorType::kBlue, m_level_info.m_blue_player_rect,
		m_level_info.m_blue_player_spawn_pos);
}

/*
 *	Dylan Goncalves Martins (D00242562)
 *	Here is checked if one of the pairs is a ray and a platform
 */
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
			auto& tile = dynamic_cast<Tile&>(*pair.second);
			const Character* player = ray_ground.m_character;

			// Check if platform underneath is valid
			if (CheckTileUnderneath(player->GetCharacterType(), tile.GetType()))
			{
				//collision found
				collide = true;
				break;
			}
		}
	}

	// can be null so it jumps out if it happens
	if (player_pair.first == nullptr || player_pair.second == nullptr)
	{
		return;
	}

	// if there was no collision
	if (!collide)
	{
		// check to see which object in pair is the ray 
		if (player_pair.first != nullptr && (player_pair.first->GetCategory() &
			Category::Type::kRay) != 0)
		{
			//call set falling
			const auto& ray_ground = dynamic_cast<RayGround&>(*player_pair.first);
			ray_ground.SetFalling();
		}
		else if (player_pair.second != nullptr && (player_pair.second->GetCategory() &
			Category::Type::kRay) != 0)
		{
			const auto& ray_ground = dynamic_cast<RayGround&>(*player_pair.second);
			ray_ground.SetFalling();
		}
	}
}

/*
 *	Dylan Goncalves Martins (D00242562)
 *	Adds every collision from one specific ray to a set
 */
void World::GetGroundRayCasts(std::set<SceneNode::Pair>& pairs, const SceneNode::Pair pair,
                              const Category::Type category) const
{
	if (pair.first->GetCategory() == category || pair.second->GetCategory() == category)
	{
		pairs.insert(std::minmax(pair.first, pair.second));
	}
}

/*
 *	Dylan Goncalves Martins (D00242562)
 *	Looks at platform underneath
 */
bool World::CheckPlatformUnderneath(const EColorType character, const EPlatformType platform)
{
	if (platform == EPlatformType::kGoal || platform == EPlatformType::kNormal)
	{
		return true;
	}

	if (character == EColorType::kRed)
	{
		if (platform == EPlatformType::kVerticalRed || platform == EPlatformType::kHorizontalRed)
		{
			return true;
		}
	}

	if (character == EColorType::kBlue)
	{
		if (platform == EPlatformType::kVerticalBlue || platform == EPlatformType::kHorizontalBlue)
		{
			return true;
		}
	}

	return false;
}


bool World::CheckTileUnderneath(const EColorType character, const ETileType tile)
{
	if (tile == kFinishPlatformPart || tile == kHorizontalPlatformPart)
	{
		return true;
	}

	if (character == EColorType::kRed)
	{
		if (tile == kVerticalRedPlatformPart || tile == kHorizontalRedPlatformPart)
		{
			return true;
		}
	}

	if (character == EColorType::kBlue)
	{
		if (tile == kVerticalBluePlatformPart || tile == kHorizontalBluePlatformPart)
		{
			return true;
		}
	}

	return false;
}
