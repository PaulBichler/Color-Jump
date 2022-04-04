#include "MultiplayerGameState.hpp"
#include "MusicPlayer.hpp"
#include "Utility.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

#include <fstream>
#include <iostream>
#include <thread>
#include <SFML/Network/Packet.hpp>
#include "GameServer.hpp"
#include "NetworkProtocol.hpp"

MultiplayerGameState::MultiplayerGameState(StateStack& stack, const Context context)
	: State(stack, context)
	  , m_world(*context.m_window, *context.m_sounds, *context.m_fonts, this)
	  , m_window(*context.m_window)
	  , m_has_focus(true)
	  , m_client_timeout(sf::seconds(2.f))
	  , m_time_since_last_packet(sf::seconds(0.f))
{
	//Build the scene
	context.m_level_manager->SetIsMultiplayer(true);
	m_world.BuildWorld(context.m_level_manager->GetCurrentLevelData());


	//Play game theme
	context.m_music->Play(MusicThemes::kMissionTheme);
}

void MultiplayerGameState::OnStackPopped()
{
	//This state is popped --> disconnect the player
	SendClientDisconnect(m_world.GetClientCharacter()->GetIdentifier());

	//SendClientDisconnect send a packet to the socket through a threaded operation.
	//Since this instance is destroyed immediately after this method is called, we need to wait
	//a bit to make sure the thread can finish.
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void MultiplayerGameState::Draw()
{
	m_world.Draw();

	//Show broadcast messages in default view
	m_window.setView(m_window.getDefaultView());
}

void MultiplayerGameState::SendPlatformInfo(const sf::Int8 player_id, const sf::Int8 platform_id,
                                            EPlatformType platform) const
{
	sf::Packet packet;
	packet << static_cast<sf::Int8>(client::PacketType::kPlatformUpdate);
	packet << player_id;
	packet << platform_id;
	packet << static_cast<sf::Int8>(platform);

	m_context.m_socket->send(packet);
}

void MultiplayerGameState::SendPlayerName(const sf::Int8 identifier, const sf::Int8 team_id,
                                          const std::string& name) const
{
	sf::Packet packet;
	packet << static_cast<sf::Int8>(client::PacketType::kPlayerUpdate);
	packet << identifier;
	packet << team_id;
	packet << name;

	m_context.m_socket->send(packet);
}


bool MultiplayerGameState::Update(const sf::Time dt)
{
	m_world.Update(dt);

	//Only handle the realtime input if the window has focus and the game is un paused
	if (m_has_focus)
	{
		CommandQueue& commands = m_world.GetCommandQueue();
		for (const auto& pair : m_players)
		{
			pair.second->HandleRealtimeInput(commands);
		}
	}

	//Handle messages from the server that may have arrived
	sf::Packet packet;
	if (m_context.m_socket->receive(packet) == sf::Socket::Done)
	{
		m_time_since_last_packet = sf::seconds(0.f);
		sf::Int8 packet_type;
		packet >> packet_type;
		HandlePacket(packet_type, packet);
	}
	else
	{
		//Check for timeout with the server
		if (m_time_since_last_packet > m_client_timeout)
		{
			m_failed_connection_text.setString("Lost connection to the server");
			Utility::CentreOrigin(m_failed_connection_text);

			m_failed_connection_clock.restart();
		}
	}

	UpdateBroadcastMessage(dt);

	//Time counter fro blinking second player text
	m_player_invitation_time += dt;
	if (m_player_invitation_time > sf::seconds(1.f))
	{
		m_player_invitation_time = sf::Time::Zero;
	}


	if (m_tick_clock.getElapsedTime() > sf::seconds(1.f / 20.f))
	{
		packet.clear();
		packet << static_cast<sf::Int8>(client::PacketType::kPositionUpdate);

		if (const Character* character = m_world.GetCharacter(m_local_player_identifier))
		{
			packet << m_local_player_identifier << character->getPosition().x << character
				->getPosition().y;
		}
		m_context.m_socket->send(packet);
		m_tick_clock.restart();
	}

	m_time_since_last_packet += dt;
	return true;
}

bool MultiplayerGameState::HandleEvent(const sf::Event& event)
{
	if (event.type == sf::Event::Closed)
	{
		//window is closed (disconnect the client)
		SendClientDisconnect(m_world.GetClientCharacter()->GetIdentifier());
	}

	//Game input handling
	CommandQueue& commands = m_world.GetCommandQueue();

	//Forward events to all players
	for (const auto& pair : m_players)
	{
		pair.second->HandleEvent(event, commands);
	}

	if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code == sf::Keyboard::Escape)
		{
			RequestStackPush(StateID::kNetworkPause);
		}
	}
	else if (event.type == sf::Event::GainedFocus)
	{
		m_has_focus = true;
	}
	else if (event.type == sf::Event::LostFocus)
	{
		m_has_focus = false;
	}
	return true;
}

void MultiplayerGameState::SendMission(const sf::Int8 player_id)
{
	sf::Packet packet;
	packet << static_cast<sf::Int8>(client::PacketType::kMission);
	packet << player_id;

	m_context.m_socket->send(packet);
}

void MultiplayerGameState::SendTeamDeath(sf::Int8 team_id)
{
	sf::Packet packet;
	packet << static_cast<sf::Int8>(client::PacketType::kTeamDeath);
	packet << team_id;

	m_socket.send(packet);
}

void MultiplayerGameState::SendCheckpointReached(sf::Int8 team_id, sf::Int8 platform_id)
{
	sf::Packet packet;
	packet << static_cast<sf::Int8>(client::PacketType::kCheckpointReached);
	packet << team_id;
	packet << platform_id;

	m_socket.send(packet);
}

void MultiplayerGameState::SendClientDisconnect(sf::Int8 identifier)
{
	sf::Packet packet;
	packet << static_cast<sf::Int8>(client::PacketType::kQuit);
	packet << identifier;

	m_socket.send(packet);
}

void MultiplayerGameState::UpdateBroadcastMessage(const sf::Time elapsed_time)
{
	if (m_broadcasts.empty())
	{
		return;
	}

	//Update broadcast timer
	m_broadcast_elapsed_time += elapsed_time;
	if (m_broadcast_elapsed_time > sf::seconds(2.f))
	{
		//If message has expired, remove it
		m_broadcasts.erase(m_broadcasts.begin());

		//Continue to display the next broadcast message
		if (!m_broadcasts.empty())
		{
			m_broadcast_text.setString(m_broadcasts.front());
			Utility::CentreOrigin(m_broadcast_text);
			m_broadcast_elapsed_time = sf::Time::Zero;
		}
	}
}

void MultiplayerGameState::HandleClientUpdate(sf::Packet& packet)
{
	sf::Int8 player_count;
	packet >> player_count;

	for (sf::Int8 i = 0; i < player_count; ++i)
	{
		sf::Vector2f position;
		sf::Int8 identifier;
		packet >> identifier >> position.x >> position.y;

		Character* character = m_world.GetCharacter(identifier);
		const bool is_local_player = identifier == m_local_player_identifier;
		if (character && !is_local_player)
		{
			sf::Vector2f interpolated_position = character->getPosition() + (
				position - character->getPosition()) * 0.5f;
			character->setPosition(interpolated_position);
		}
	}
}

void MultiplayerGameState::HandleSelfSpawn(sf::Packet& packet)
{
	sf::Int8 identifier;
	sf::Int8 team_id;
	std::string name;

	sf::Int8 players;
	packet >> players;

	for (int i = 0; i < players; ++i)
	{
		packet >> identifier >> team_id >> name;

		const auto ghost = m_world.AddGhostCharacter(identifier);

		ghost->SetTeamIdentifier(team_id);
		ghost->SetName(name);

		m_players[identifier].reset(new Player(m_context.m_socket, identifier, nullptr));
	}

	packet >> identifier >> team_id >> name;


	const auto character = m_world.AddCharacter(identifier, true);
	character->SetTeamIdentifier(team_id);
	character->SetName(name);
	m_players[identifier].reset(new Player(m_context.m_socket, identifier, GetContext().m_keys1));
	m_local_player_identifier = identifier;


	

}

void MultiplayerGameState::HandleBroadcast(sf::Packet& packet)
{
	std::string message;
	packet >> message;
	m_broadcasts.push_back(message);

	//Just added the first message, display immediately
	if (m_broadcasts.size() == 1)
	{
		m_broadcast_text.setString(m_broadcasts.front());
		Utility::CentreOrigin(m_broadcast_text);
		m_broadcast_elapsed_time = sf::Time::Zero;
	}
}

void MultiplayerGameState::HandlePlayerConnect(sf::Packet& packet)
{
	sf::Int8 identifier;
	packet >> identifier;

	m_world.AddGhostCharacter(identifier);
	m_world.UpdateCharacters();
	m_players[identifier].reset(new Player(m_context.m_socket, identifier, nullptr));
}

void MultiplayerGameState::HandlePlayerDisconnect(sf::Packet& packet)
{
	sf::Int8 identifier;
	packet >> identifier;
	m_world.RemoveCharacter(identifier);
	m_players.erase(identifier);
}

void MultiplayerGameState::HandleInitialState(sf::Packet& packet)
{
	sf::Int8 player_count;
	packet >> player_count;
	for (sf::Int8 i = 0; i < player_count; ++i)
	{
		sf::Int8 identifier;
		sf::Int8 team_identifier;
		sf::Vector2f position;
		std::string name;

		packet >> identifier >> position.x >> position.y >> team_identifier >> name;

		Character* character = m_world.AddGhostCharacter(identifier);
		character->setPosition(position);
		character->SetTeamIdentifier(team_identifier);
		character->SetName(name);
		m_players[identifier].reset(new Player(m_context.m_socket, identifier, nullptr));
	}
}

void MultiplayerGameState::HandleUpdatePlatformColors(sf::Packet& packet)
{
	sf::Int8 player_id;
	sf::Int8 platform_id;
	sf::Int8 platform_color;

	packet >> player_id >> platform_id >> platform_color;

	const auto client_char = m_world.GetClientCharacter();
	const auto send_char = m_world.GetCharacter(player_id);

	if (client_char->GetTeamIdentifier() == send_char->GetTeamIdentifier())
	{
		if (client_char->GetIdentifier() != player_id)
		{
			m_world.SetPlatformOnCharacter(send_char, platform_id);
			m_world.SetTeammate(send_char);
		}

		m_world.UpdatePlatform(send_char->GetIdentifier(), platform_id,
		                       static_cast<EPlatformType>(platform_color));
	}
}

void MultiplayerGameState::HandleUpdatePlayer(sf::Packet& packet) const
{
	sf::Int8 identifier;
	sf::Int8 team_id;
	std::string name;
	packet >> identifier >> team_id >> name;
	m_world.GetCharacter(identifier)->SetName(name);
	m_world.GetCharacter(identifier)->SetTeamIdentifier(team_id);
}

void MultiplayerGameState::HandleMission(sf::Packet& packet) const
{
	sf::Int8 team_id;
	packet >> team_id;

	if (team_id == m_world.GetClientCharacter()->GetTeamIdentifier())
	{
		RequestStackPush(StateID::kLevelWin);
	}
	else
	{
		RequestStackPush(StateID::kLevelLose);
	}
}

void MultiplayerGameState::HandleTeamRespawn(sf::Packet& packet) const
{
	sf::Int8 team_id;
	packet >> team_id;

	if(m_world.GetClientCharacter()->GetTeamIdentifier() == team_id)
	{
		m_world.RespawnClientCharacter();
	}
}

void MultiplayerGameState::HandleTeamCheckpointSet(sf::Packet& packet)
{
	sf::Int8 team_id;
	sf::Int8 platform_id;
	packet >> team_id;
	packet >> platform_id;

	if (m_world.GetClientCharacter()->GetTeamIdentifier() == team_id)
	{
		m_world.SetCheckpointToPlatformWithID(platform_id);
	}
}

void MultiplayerGameState::HandlePacket(sf::Int8 packet_type, sf::Packet& packet)
{
	switch (static_cast<server::PacketType>(packet_type))
	{
	//Send message to all Clients
	case server::PacketType::kBroadcastMessage:
		HandleBroadcast(packet);
		break;
	case server::PacketType::kSpawnSelf:
		HandleSelfSpawn(packet);
		break;
	case server::PacketType::kPlayerConnect:
		HandlePlayerConnect(packet);
		break;
	case server::PacketType::kPlayerDisconnect:
		HandlePlayerDisconnect(packet);
		break;
	case server::PacketType::kInitialState:
		HandleInitialState(packet);
		break;
	case server::PacketType::kMissionSuccess:
		HandleMission(packet);
		break;
	case server::PacketType::kUpdateClientState:
		HandleClientUpdate(packet);
		break;
	case server::PacketType::kUpdatePlatformColors:
		HandleUpdatePlatformColors(packet);
		break;
	case server::PacketType::kUpdatePlayer:
		HandleUpdatePlayer(packet);
		break;
	case server::PacketType::kRespawnTeam:
		HandleTeamRespawn(packet);
		break;
	case server::PacketType::kSetTeamCheckpoint:
		HandleTeamCheckpointSet(packet);
		break;
	default:
		break;
	}
}
