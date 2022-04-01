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
	m_camera.setCenter(m_camera.getSize().x / 2.f,
	                   m_world_bounds.height - m_camera.getSize().y / 2.f);
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

void World::LoadLevel(LevelManager::LevelData current_level_data)
{
	LevelLoader level_loader(current_level_data, m_textures, m_sounds);
	m_level_info = level_loader.LoadLevel();
}

//Written by Paul Bichler (D00242563)
void World::BuildWorld(LevelManager::LevelData current_level_data)
{
	//Load the level based on the level data in the level manager 
	LoadLevel(std::move(current_level_data));

	m_scene_layers[static_cast<int>(Layers::kBackground)]->AttachChild(
		std::move(m_level_info.background_parent));
	m_scene_layers[static_cast<int>(Layers::kLevel)]->AttachChild(
		std::move(m_level_info.level_parent));
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

Character* World::AddCharacterWithColor(const sf::Int32 identifier,
                                        const EColorType color, const sf::IntRect rect,
                                        const sf::Vector2f spawn_pos)
{
	std::unique_ptr<Character> player(
		new Character(color, m_textures, rect, m_sounds));
	player->setPosition(spawn_pos);
	player->SetIdentifier(identifier);
	player->SetTeamIdentifier((identifier + 1) / 2);

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
