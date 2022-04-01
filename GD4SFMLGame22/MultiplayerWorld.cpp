#include "MultiplayerWorld.hpp"
#include "CollisionHandler.hpp"
#include "GhostCharacter.hpp"
#include "PlatformPart.hpp"
#include "Utility.hpp"

MultiplayerWorld::MultiplayerWorld(sf::RenderTarget& output_target, SoundPlayer& sounds)
	: World(output_target, sounds)
{
}

void MultiplayerWorld::Update(const sf::Time dt)
{
	World::Update(dt);
}

void MultiplayerWorld::Draw()
{
	World::Draw();
}

Character* MultiplayerWorld::GetCharacter(const sf::Int32 identifier) const
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

void MultiplayerWorld::RemoveCharacter(const sf::Int32 identifier)
{
	Character* character = GetCharacter(identifier);
	if (character)
	{
		character->Destroy();
		m_players.erase(std::find(m_players.begin(), m_players.end(), character));
	}
}

Character* MultiplayerWorld::AddGhostCharacterWithColor(const sf::Int32 identifier, const EColorType color,
                                                        const sf::IntRect& int_rect,
                                                        const sf::Vector2f& spawn_pos)
{
	std::unique_ptr<GhostCharacter> player(
		new GhostCharacter(color, m_textures, int_rect, m_sounds));
	player->setPosition(spawn_pos);
	player->SetIdentifier(identifier);
	player->SetTeamIdentifier((identifier + 1) / 2);

	m_players.emplace_back(player.get());
	m_scene_layers[static_cast<int>(Layers::kCharacters)]->AttachChild(std::move(player));
	return m_players.back();
}

Character* MultiplayerWorld::AddGhostCharacter(const sf::Int32 identifier)
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

void MultiplayerWorld::SetCamera()
{
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
		player_pairs.insert(
			std::pair<Character*, std::set<SceneNode::Pair>>(players, std::set<SceneNode::Pair>()));
	}

	for (const SceneNode::Pair& pair : collision_pairs)
	{
		if (CollisionHandler::PlatformCollision(pair, m_players, m_reached_goal_callback)) continue;

		//Get All Ground Ray Casts for player one and two
		CollisionHandler::GetGroundRayCasts(player_pairs, pair, Category::kRay);
	}

	CollisionHandler::PlayerGroundRayCast(player_pairs);
}

sf::FloatRect MultiplayerWorld::GetBattlefieldBounds() const
{
	return {};
}
