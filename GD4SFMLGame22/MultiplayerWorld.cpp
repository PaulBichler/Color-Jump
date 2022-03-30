#include "MultiplayerWorld.hpp"

#include <iostream>

#include "MultiplayerLevelLoader.hpp"
#include "PlatformPart.hpp"

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

Character* MultiplayerWorld::AddCharacterWithColor(const sf::Int32 identifier,
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


Character* MultiplayerWorld::AddCharacter(const sf::Int32 identifier)
{
	if (identifier % 2 == 0)
	{
		return AddCharacterWithColor(identifier, EColorType::kRed, m_level_info.m_red_player_rect,
		                             m_level_info.m_red_player_spawn_pos);
	}

	return AddCharacterWithColor(identifier, EColorType::kBlue, m_level_info.m_blue_player_rect,
	                             m_level_info.m_blue_player_spawn_pos);
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

LevelInfo& MultiplayerWorld::BuildLevel(LevelManager::LevelData current_level_data)
{
	std::cout << "Load Level" << std::endl;
	//This method is called in the constructor of the base World class
	SingleplayerLevelLoader level_loader(current_level_data, m_textures, m_sounds);
	m_level_info = level_loader.LoadLevel();
	std::cout << "Level Loaded" << std::endl;

	return m_level_info;
}

void MultiplayerWorld::SetCamera()
{
}

bool MultiplayerWorld::HandlePlayerTileCollision(SceneNode::Pair pair)
{
	if (MatchesCategories(pair, Category::Type::kPlayer, Category::Type::kPlatform))
	{
		auto& player = dynamic_cast<Character&>(*pair.first);
		auto& tile = dynamic_cast<Tile&>(*pair.second);


		//Checks if player collided from underneath the center of the platform
		if (IsPlayerBelowTile(player, tile))
		{
			//Checks if platform is collidable with player
			if (IsPlayerAtHisTile(player, tile))
			{
				// move player out of collision and stop his movement
				player.MoveOutOfCollision(tile.GetBoundingRect());
				player.StopMovement();

				// Set color of vertical platform if there is a collision from the side/underneath 
				if (tile.GetType() == kVerticalImpactPlatformPart)
				{
					tile.HandleCollision(player.GetCharacterType());
				}
			}
			// continue to next pair
			return true;
		}

		//Ground players
		if (tile.HandleCollision(player.GetCharacterType()))
		{
			//Collision
			player.SetGrounded();
		}
	}
	return false;
}

void MultiplayerWorld::HandleCollisions()
{
	std::set<SceneNode::Pair> collision_pairs;
	m_sceneGraph.CheckSceneCollision(m_sceneGraph, collision_pairs, [this](SceneNode& node)
	{
		//check collisions only for players and RayGround objects
		return dynamic_cast<Character*>(&node) != nullptr || dynamic_cast<RayGround*>(&node) != nullptr;
	});

	std::set<SceneNode::Pair> pairs_player_one;
	std::set<SceneNode::Pair> pairs_player_two;

	for (const SceneNode::Pair& pair : collision_pairs)
	{
		if (HandlePlayerTileCollision(pair)) continue;

		//Get All Ground Ray Casts for player one and two
		GetGroundRayCasts(pairs_player_one, pair, Category::kRayOne);
		GetGroundRayCasts(pairs_player_two, pair, Category::kRayTwo);
	}

	//Check Ground Ray Casts
	PlayerGroundRayCast(pairs_player_one);
	PlayerGroundRayCast(pairs_player_two);
}

sf::FloatRect MultiplayerWorld::GetBattlefieldBounds() const
{
	return {};
}
