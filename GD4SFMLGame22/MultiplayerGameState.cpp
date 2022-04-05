#include "MultiplayerGameState.hpp"

#include <fstream>
#include <thread>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Network/Packet.hpp>

#include "GameServer.hpp"
#include "MusicPlayer.hpp"
#include "NetworkProtocol.hpp"
#include "Utility.hpp"

MultiplayerGameState::MultiplayerGameState(StateStack& stack, Context& context)
	: State(stack, context)
	  , m_world(*context.m_window, *context.m_sounds, *context.m_fonts, this)
	  , m_connected(true)
	  , m_has_focus(true)
	  , m_client_timeout(sf::seconds(2.f))
	  , m_time_since_last_packet(sf::seconds(0.f))
{
	const int y = m_context.m_window->getSize().y / 2;
	const int x = m_context.m_window->getSize().x / 2;
	Utility::CreateLabel(m_context, m_failed_connection_text, x, y, "Attempting to connect...", 35);
	Utility::CentreOrigin(m_failed_connection_text->GetText());
	m_gui_fail_container.Pack(m_failed_connection_text);

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

	//Disable the server (does nothing if you're not the host)
	GetContext().DisableServer();

	//SendClientDisconnect send a packet to the socket through a threaded operation.
	//Since this instance is destroyed immediately after this method is called, we need to wait
	//a bit to make sure the thread can finish.
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void MultiplayerGameState::Draw()
{
	if (m_connected)
	{
		m_world.Draw();
	}
	else
	{
		sf::RenderWindow& window = *GetContext().m_window;
		window.clear(sf::Color(0, 37, 97));
		window.draw(m_gui_fail_container);
	}
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

void MultiplayerGameState::SendPlayerName(const sf::Int8 identifier, const std::string& name) const
{
	sf::Packet packet;
	packet << static_cast<sf::Int8>(client::PacketType::kPlayerUpdate);
	packet << identifier;
	packet << name;
	Debug("Name change.");
	m_context.m_socket->send(packet);
}


bool MultiplayerGameState::Update(const sf::Time dt)
{
	if (m_connected)
	{
		m_world.Update(dt);

		if (m_has_focus)
		{
			CommandQueue& commands = m_world.GetCommandQueue();
			for (auto& pair : m_players)
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
				m_connected = false;
				m_failed_connection_text->SetText("Lost connection to the server");
				Utility::CentreOrigin(m_failed_connection_text->GetText());
				m_failed_connection_clock.restart();
			}
		}

		if (m_tick_clock.getElapsedTime() > sf::seconds(1.f / 20.f))
		{
			packet.clear();
			packet << static_cast<sf::Int8>(client::PacketType::kPositionUpdate);
			const Character* character = m_world.GetCharacter(m_local_player_identifier);
			packet << m_local_player_identifier << character->getPosition().x << character->
				getPosition().y;
			m_context.m_socket->send(packet);
			m_tick_clock.restart();
		}
		m_time_since_last_packet += dt;
	}
	else if (m_failed_connection_clock.getElapsedTime() >= sf::seconds(5.f))
	{
		RequestStackClear();
		RequestStackPush(StateID::kMenu);
	}

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

	Debug("Mission completed.");

	m_context.m_socket->send(packet);
}

void MultiplayerGameState::SendTeamDeath(sf::Int8 team_id)
{
	sf::Packet packet;
	packet << static_cast<sf::Int8>(client::PacketType::kTeamDeath);
	packet << team_id;
	Debug("Team died.");
	m_context.m_socket->send(packet);
}

void MultiplayerGameState::SendCheckpointReached(sf::Int8 team_id, sf::Int8 platform_id) const
{
	sf::Packet packet;
	packet << static_cast<sf::Int8>(client::PacketType::kCheckpointReached);
	packet << team_id;
	packet << platform_id;
	Debug("Checkpoint reached.");
	m_context.m_socket->send(packet);
}

void MultiplayerGameState::SendClientDisconnect(sf::Int8 identifier) const
{
	sf::Packet packet;
	Debug("Client disconnected.");
	packet << static_cast<sf::Int8>(client::PacketType::kQuit);
	packet << identifier;

	m_context.m_socket->send(packet);
}

void MultiplayerGameState::HandleClientUpdate(sf::Packet& packet) const
{
	Debug("Handle client update.");

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

void MultiplayerGameState::Debug(const std::string& message)
{
	Utility::Debug("MultiplayerGameState: " + message);
}


void MultiplayerGameState::HandleSelfSpawn(sf::Packet& packet)
{
	Debug("Self spawn.");
	sf::Int8 identifier;
	sf::Int8 team_id;
	sf::Int8 color;
	std::string name;

	sf::Int8 players;
	packet >> players;

	for (int i = 0; i < players; ++i)
	{
		packet >> identifier >> team_id >> color >> name;

		const auto ghost = m_world.AddGhostCharacter(identifier, color);

		ghost->SetTeamIdentifier(team_id);
		ghost->SetName(name);

		m_players[identifier].reset(new Player(m_context.m_socket.get(), identifier, nullptr));
	}

	packet >> identifier >> team_id >> color >> name;

	m_local_player_identifier = identifier;
	const auto character = m_world.AddCharacter(identifier, color, true);
	character->SetTeamIdentifier(team_id);
	character->SetName(name);
	m_players[identifier].reset(new Player(m_context.m_socket.get(), identifier,
	                                       GetContext().m_keys1));

	m_world.UpdateCharacters(team_id);
}

void MultiplayerGameState::HandlePlayerDisconnect(sf::Packet& packet)
{
	Debug("Player disconnected.");

	sf::Int8 identifier;
	packet >> identifier;
	m_world.RemoveCharacter(identifier);
	m_players.erase(identifier);
}

void MultiplayerGameState::HandleUpdatePlatformColors(sf::Packet& packet)
{
	Debug("Update platform colors.");

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
	Debug("Update player.");

	sf::Int8 identifier;
	std::string name;
	packet >> identifier >> name;
	m_world.GetCharacter(identifier)->SetName(name);
}

void MultiplayerGameState::HandleMission(sf::Packet& packet) const
{
	Debug("Handle mission.");

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
	Debug("Team respawn.");
	sf::Int8 team_id;
	packet >> team_id;

	if (m_world.GetClientCharacter()->GetTeamIdentifier() == team_id)
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
	case server::PacketType::kSpawnSelf:
		HandleSelfSpawn(packet);
		break;
	case server::PacketType::kPlayerDisconnect:
		HandlePlayerDisconnect(packet);
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
