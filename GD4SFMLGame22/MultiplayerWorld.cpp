#include "MultiplayerWorld.hpp"
#include "CollisionHandler.hpp"
#include "GhostCharacter.hpp"
#include "PlatformPart.hpp"
#include "Utility.hpp"

MultiplayerWorld::MultiplayerWorld(sf::RenderTarget& output_target, SoundPlayer& sounds, FontHolder& fonts, MultiplayerGameState* state)
	: World(output_target, sounds, fonts),
	  m_client_player(nullptr),
	  m_state(state)
{
	m_reached_goal_callback = [this] { OnReachedGoal(); };
}

void MultiplayerWorld::Update(const sf::Time dt)
{
	World::Update(dt);

	if(m_client_player != nullptr)
	{
		sf::Vector2f camera_pos = m_camera.getCenter();
		camera_pos.x = m_client_player->getPosition().x;
		m_camera.setCenter(camera_pos);
	}
}

void MultiplayerWorld::Draw()
{
	World::Draw();
}

Character* MultiplayerWorld::GetCharacter(const sf::Int8 identifier) const
{
	for (Character* character : m_players)
	{
		if (character->GetIdentifier() == identifier)
		{
			return character;
		}
	}
	return nullptr;
}

void MultiplayerWorld::RemoveCharacter(const sf::Int8 identifier)
{
	Character* character = GetCharacter(identifier);

	if (character)
	{
		character->Destroy();
		m_players.erase(std::find(m_players.begin(), m_players.end(), character));
	}
}

Character* MultiplayerWorld::AddGhostCharacterWithColor(const sf::Int8 identifier,
                                                        const EColorType color,
                                                        const sf::IntRect& int_rect,
                                                        const sf::Vector2f& spawn_pos)
{
	std::unique_ptr<GhostCharacter> player(new GhostCharacter(color, m_textures, m_fonts, int_rect, m_sounds));
	player->setPosition(spawn_pos);
	player->SetIdentifier(identifier);
	player->SetTeamIdentifier((identifier + 1) / 2);

	m_players.emplace_back(player.get());
	m_scene_layers[static_cast<int>(Layers::kCharacters)]->AttachChild(std::move(player));
	return m_players.back();
}

Character* MultiplayerWorld::AddGhostCharacter(const sf::Int8 identifier)
{
	if (identifier % 2 == 0)
	{
		return AddGhostCharacterWithColor(identifier, EColorType::kRed,
		                                  m_level_info.m_red_player_rect,
		                                  m_level_info.m_red_player_spawn_pos);
	}

	return AddGhostCharacterWithColor(identifier, EColorType::kBlue,
	                                  m_level_info.m_blue_player_rect,
	                                  m_level_info.m_blue_player_spawn_pos);
}

void MultiplayerWorld::UpdatePlatform(const sf::Int8 platform_id, const EPlatformType platform_color) const
{
	// Get all platforms
	// Set the new one to the correct color

	for (const auto& platform : m_level_info.platforms)
	{
		if (platform->GetID() == platform_id)
		{
			platform->SetType(platform_color);
		}
	}
}

Character* MultiplayerWorld::AddCharacter(const sf::Int8 identifier, const bool is_client_player)
{
	Character* player_character = World::AddCharacter(identifier, is_client_player);

	if (is_client_player)
		m_client_player = player_character;

	return player_character;
}

Character* MultiplayerWorld::GetClientCharacter() const
{
	return m_client_player;
}

void MultiplayerWorld::SetPlatformOnCharacter(Character* character, sf::Int8 platform_id) const
{
	for (const auto& platform : m_level_info.platforms)
	{
		if (platform->GetID() == platform_id)
		{
			character->SetGrounded(platform.get());
			break;
		}
	}
}

void MultiplayerWorld::SetTeammate(Character* character)
{
	m_team_mate = character;
}

Character* MultiplayerWorld::GetTeammate() const
{
	return m_team_mate;
}

void MultiplayerWorld::UpdatePlatformColors(const std::map<sf::Int8, sf::Int8>& platform_colors) const
{
	for (auto& value : platform_colors)
	{
		for (const auto& platform : m_level_info.platforms)
		{
			if (platform->GetID() == value.first)
			{
				platform->SetType(static_cast<EPlatformType>(value.second));
			}
		}
	}
}

void MultiplayerWorld::HandleCollisions()
{
	std::set<SceneNode::Pair> collision_pairs;
	m_sceneGraph.CheckSceneCollision(m_sceneGraph, collision_pairs, [this](SceneNode& node)
	{
		const auto character = dynamic_cast<Character*>(&node);
		const bool character_cond = character != nullptr;

		const auto ray_ground = dynamic_cast<RayGround*>(&node);
		const bool ray_cond = ray_ground != nullptr;

		//check collisions only for players and RayGround objects
		return character_cond || ray_cond;
	});

	std::set<SceneNode::Pair> player_pair;

	for (const SceneNode::Pair& pair : collision_pairs)
	{
		if (CollisionHandler::PlatformCollision(pair, m_reached_goal_callback, this))
			continue;

		//Get All Ground Ray Casts for player one and two
		CollisionHandler::GetGroundRayCasts(player_pair, pair, Category::kRay);
	}

	CollisionHandler::PlayerGroundRayCast(player_pair);
}

sf::FloatRect MultiplayerWorld::GetBattlefieldBounds() const
{
	return {};
}

void MultiplayerWorld::OnReachedGoal()
{
	Utility::Debug("Checkpoint Reached!");
}
