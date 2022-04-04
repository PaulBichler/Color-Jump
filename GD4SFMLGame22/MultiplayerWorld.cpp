#include "MultiplayerWorld.hpp"
#include "CollisionHandler.hpp"
#include "GhostCharacter.hpp"
#include "MultiplayerGameState.hpp"
#include "PlatformPart.hpp"
#include "Utility.hpp"

MultiplayerWorld::MultiplayerWorld(sf::RenderTarget& output_target, SoundPlayer& sounds, FontHolder& fonts, MultiplayerGameState* state)
	: World(output_target, sounds, fonts),
	  m_checkpoint(nullptr),
	  m_client_player(nullptr),
	  m_team_mate(nullptr),
	  m_state(state)
{
}

void MultiplayerWorld::Update(const sf::Time dt)
{
	World::Update(dt);

	if (m_client_player != nullptr)
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

void MultiplayerWorld::UpdatePlatform(const sf::Int8 id, const sf::Int8 platform_id, const EPlatformType platform_color)
{
	for (auto& platform : m_level_info.platforms)
	{
		if (platform->GetID() == platform_id)
		{
			if (platform_color == EPlatformType::kCheckpointActivated)
			{
				if (GetClientCharacter()->GetIdentifier() == id)
				{
					platform->SetType(platform_color);
				}
			}
			else
			{
				platform->SetType(platform_color);
			}

			//Initialize the first checkpoint (spawn platform)
			if (m_checkpoint == nullptr)
				m_checkpoint = platform.get();
		}
	}
}

Character* MultiplayerWorld::AddCharacter(const sf::Int8 identifier, const bool is_client_player)
{
	Character* player_character = World::AddCharacter(identifier, is_client_player);

	if (is_client_player)
	{
		m_client_player = player_character;
		UpdateCharacters(player_character->GetTeamIdentifier());
	}

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

void MultiplayerWorld::UpdateCharacters(const sf::Int8 team_id) const
{
	for (Character* player : m_players)
	{
		if (player->GetTeamIdentifier() != team_id)
		{
			player->SetColor();
		}
	}
}

void MultiplayerWorld::UpdateCharacters() const
{
	if (GetClientCharacter() != nullptr)
	{
		UpdateCharacters(GetClientCharacter()->GetTeamIdentifier());
	}
}

void MultiplayerWorld::RespawnClientCharacter() const
{
	//Reset the position of the player to the position of the last checkpoint
	m_client_player->StopMovement();
	const auto& parts = m_checkpoint->GetParts();
	PlatformPart* part = parts[parts.size() / 2];
	sf::Vector2f position = part->getPosition();
	position.y -= m_client_player->GetSize().height - 15.f;
	position.x += part->GetSize().width / 2;
	m_client_player->setPosition(position);

	//Reset all platforms to their initial type
	for (auto& platform : m_level_info.platforms)
	{
		platform->ResetToInitialType();
	}
}

void MultiplayerWorld::SetCheckpointToPlatformWithID(const sf::Int8 platform_id)
{
	Platform* new_checkpoint = m_level_info.GetPlatformWithID(platform_id);

	if(new_checkpoint != nullptr)
	{
		m_checkpoint = new_checkpoint;
		m_checkpoint->SetType(EPlatformType::kCheckpointActivated);
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
		if (CollisionHandler::PlatformCollision(pair, [this] { OnReachedCheckpoint(); }, [this] { OnReachedGoal(); }, this))
			continue;

		CollisionHandler::TrapCollision(pair, [this] { OnClientPlayerDeath(); });

		//Get All Ground Ray Casts for player one and two
		CollisionHandler::GetGroundRayCasts(player_pair, pair, Category::kRay);
	}

	CollisionHandler::PlayerGroundRayCast(player_pair);
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

void MultiplayerWorld::OnReachedCheckpoint() const
{
	Platform* current_platform = m_client_player->GetCurrentPlatform();

	if(current_platform == m_team_mate->GetCurrentPlatform() && current_platform != m_checkpoint)
		m_state->SendCheckpointReached(m_client_player->GetTeamIdentifier(), current_platform->GetID());

	/*m_checkpoint = m_client_player->GetCurrentPlatform();
	m_checkpoint->SetType(EPlatformType::kCheckpointActivated);*/
}

void MultiplayerWorld::OnReachedGoal() const
{
	m_state->SendMission(GetClientCharacter()->GetTeamIdentifier());
}

void MultiplayerWorld::OnClientPlayerDeath() const
{
	m_state->SendTeamDeath(m_client_player->GetTeamIdentifier());
}
