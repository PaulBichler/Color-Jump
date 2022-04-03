#include "MultiplayerWorld.hpp"
#include "CollisionHandler.hpp"
#include "GhostCharacter.hpp"
#include "PlatformPart.hpp"
#include "Utility.hpp"

MultiplayerWorld::MultiplayerWorld(sf::RenderTarget& output_target, SoundPlayer& sounds, FontHolder& fonts, MultiplayerGameState* state)
	: World(output_target, sounds, fonts),
	  m_checkoint(nullptr),
	  m_client_player(nullptr),
	  m_team_mate(nullptr),
	  m_state(state)
{
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

void MultiplayerWorld::UpdatePlatform(const sf::Int8 platform_id, const EPlatformType platform_color)
{
	// Get all platforms
	// Set the new one to the correct color

	for (auto& platform : m_level_info.platforms)
	{
		if (platform->GetID() == platform_id)
		{
			platform->SetType(platform_color);

			//Initialize the first checkpoint (spawn platform)
			if (m_checkoint == nullptr)
				m_checkoint = platform.get();
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

	std::map<Character*, std::set<SceneNode::Pair>> player_pairs;

	for (auto players : m_players)
	{
		player_pairs.insert(std::pair<Character*, std::set<SceneNode::Pair>>(players, std::set<SceneNode::Pair>()));
	}

	for (const SceneNode::Pair& pair : collision_pairs)
	{
		if (CollisionHandler::PlatformCollision(pair, m_players, [this] { OnReachedCheckpoint(); }, this))
			continue;

		CollisionHandler::TrapCollision(pair, [this] { OnClientPlayerDeath(); });

		//Get All Ground Ray Casts for player one and two
		CollisionHandler::GetGroundRayCasts(player_pairs, pair, Category::kRay);
	}

	CollisionHandler::PlayerGroundRayCast(player_pairs);
}

sf::FloatRect MultiplayerWorld::GetBattlefieldBounds() const
{
	return {};
}

void MultiplayerWorld::DestroyEntitiesOutsideView()
{
	Command command;
	command.category = Category::Type::kPlayer;
	command.action = DerivedAction<Entity>([this](Entity& e, sf::Time)
	{
		if (e.getPosition().y > m_camera.getCenter().y + m_camera.getSize().y / 2)
		{
			if (&e == static_cast<Entity*>(m_client_player))
			{
				OnClientPlayerDeath();
			}
		}
	});

	m_command_queue.Push(command);
}

void MultiplayerWorld::OnReachedCheckpoint()
{
	m_checkoint = m_client_player->GetCurrentPlatform();
	m_checkoint->SetType(EPlatformType::kCheckpointActivated);
}

void MultiplayerWorld::OnClientPlayerDeath() const
{
	m_client_player->StopMovement();
	const auto& parts = m_checkoint->GetParts();
	PlatformPart* part = parts[parts.size() / 2];
	sf::Vector2f position = part->getPosition();
	position.y += m_client_player->GetSize().height;
	position.x += part->GetSize().width / 2;
	m_client_player->setPosition(position);
}
