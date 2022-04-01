#include "MultiplayerGameState.hpp"
#include "MusicPlayer.hpp"
#include "Utility.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Network/IpAddress.hpp>

#include <fstream>
#include <iostream>
#include <SFML/Network/Packet.hpp>
#include "GameServer.hpp"

sf::IpAddress GetAddressFromFile()
{
	{
		//Try to open existing file ip.txt
		std::ifstream input_file("ip.txt");
		std::string ip_address;
		if (input_file >> ip_address)
		{
			return ip_address;
		}
	}

	//If open/read failed, create a new file
	std::ofstream output_file("ip.txt");
	std::string local_address = "127.0.0.1";
	output_file << local_address;
	return local_address;
}

MultiplayerGameState::MultiplayerGameState(StateStack& stack, const Context context,
                                           const bool is_host)
	: State(stack, context)
	  , m_world(*context.m_window, *context.m_sounds)
	  , m_window(*context.m_window)
	  , m_texture_holder(*context.m_textures)
	  , m_connected(false)
	  , m_game_server(nullptr)
	  , m_active_state(true)
	  , m_has_focus(true)
	  , m_host(is_host)
	  , m_game_started(false)
	  , m_client_timeout(sf::seconds(2.f))
	  , m_time_since_last_packet(sf::seconds(0.f))
{
	m_broadcast_text.setFont(context.m_fonts->Get(Fonts::Main));
	m_broadcast_text.setPosition(1024.f / 2, 100.f);

	//We reuse this text for "Attempt to connect" and "Failed to connect" messages
	m_failed_connection_text.setFont(context.m_fonts->Get(Fonts::Main));
	m_failed_connection_text.setString("Attempting to connect...");
	m_failed_connection_text.setCharacterSize(35);
	m_failed_connection_text.setFillColor(sf::Color::White);
	Utility::CentreOrigin(m_failed_connection_text);
	m_failed_connection_text.setPosition(m_window.getSize().x / 2.f, m_window.getSize().y / 2.f);

	//Render an "establishing connection" frame for user feedback
	m_window.clear(sf::Color::Black);
	m_window.draw(m_failed_connection_text);
	m_window.display();
	m_failed_connection_text.setString("Could not connect to the remote server");
	Utility::CentreOrigin(m_failed_connection_text);

	sf::IpAddress ip;
	if (m_host)
	{
		m_game_server.reset(new GameServer(sf::Vector2f(m_window.getSize())));
		ip = "127.0.0.1";
		Utility::Debug("New server on");
	}
	else
	{
		ip = GetAddressFromFile();
	}

	if (sf::TcpSocket::Done == m_socket.connect(ip, SERVER_PORT, sf::seconds(60.f)))
	{
		m_connected = true;
	}
	else
	{
		m_failed_connection_clock.restart();
	}

	m_socket.setBlocking(false);

	//Build the scene
	context.m_level_manager->SetIsMultiplayer(true);
	m_world.BuildWorld(context.m_level_manager->GetCurrentLevelData());

	//Play game theme
	context.m_music->Play(MusicThemes::kMissionTheme);
}

void MultiplayerGameState::Draw()
{
	if (m_connected)
	{
		m_world.Draw();

		//Show broadcast messages in default view
		m_window.setView(m_window.getDefaultView());

		if (!m_broadcasts.empty())
		{
			m_window.draw(m_broadcast_text);
		}

		if (m_local_player_identifiers.size() < 2 && m_player_invitation_time < sf::seconds(0.5f))
		{
			m_window.draw(m_player_invitation_text);
		}
	}
	else
	{
		m_window.draw(m_failed_connection_text);
	}
}

bool MultiplayerGameState::Update(const sf::Time dt)
{
	//Connected to the Server: Handle all the network logic
	if (m_connected)
	{
		m_world.Update(dt);

		//Only handle the realtime input if the window has focus and the game is un paused
		if (m_active_state && m_has_focus)
		{
			CommandQueue& commands = m_world.GetCommandQueue();
			for (const auto& pair : m_players)
			{
				pair.second->HandleRealtimeInput(commands);
			}
		}

		//Always handle the network input
		CommandQueue& commands = m_world.GetCommandQueue();
		for (const auto& pair : m_players)
		{
			pair.second->HandleRealtimeNetworkInput(commands);
		}

		//Handle messages from the server that may have arrived
		sf::Packet packet;
		if (m_socket.receive(packet) == sf::Socket::Done)
		{
			m_time_since_last_packet = sf::seconds(0.f);
			sf::Int32 packet_type;
			packet >> packet_type;
			HandlePacket(packet_type, packet);
		}
		else
		{
			//Check for timeout with the server
			if (m_time_since_last_packet > m_client_timeout)
			{
				m_connected = false;
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
			sf::Packet position_packet;
			position_packet << static_cast<sf::Int32>(client::PacketType::kPositionUpdate);
			position_packet << static_cast<sf::Int32>(m_local_player_identifiers.size());

			for (const sf::Int32 identifier : m_local_player_identifiers)
			{
				if (const Character* character = m_world.GetCharacter(identifier))
				{
					position_packet << identifier << character->getPosition().x << character->
						getPosition().y;
				}
			}
			m_socket.send(position_packet);
			m_tick_clock.restart();
		}

		m_time_since_last_packet += dt;
	}

	//Failed to connect and waited for more than 5 seconds: Back to menu
	else if (m_failed_connection_clock.getElapsedTime() >= sf::seconds(5.f))
	{
		RequestStackClear();
		RequestStackPush(StateID::kMenu);
	}
	return true;
}

bool MultiplayerGameState::HandleEvent(const sf::Event& event)
{
	//Game input handling
	CommandQueue& commands = m_world.GetCommandQueue();

	//Forward events to all players
	for (const auto& pair : m_players)
	{
		pair.second->HandleEvent(event, commands);
	}

	if (event.type == sf::Event::KeyPressed)
	{
		if (m_connected)
		{
			//If escape is pressed, show the pause screen
			if (event.key.code == sf::Keyboard::Escape)
			{
				DisableAllRealtimeActions();
				RequestStackPush(StateID::kNetworkPause);
			}
		}
		else
		{
			if (event.key.code == sf::Keyboard::Escape)
			{
				RequestStackPop();
				RequestStackPush(StateID::kMenu);
			}
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

void MultiplayerGameState::OnActivate()
{
	m_active_state = true;
}

void MultiplayerGameState::OnDestroy()
{
	if (!m_host && m_connected)
	{
		//Inform server this client is dying
		sf::Packet packet;
		packet << static_cast<sf::Int32>(client::PacketType::kQuit);
		m_socket.send(packet);
	}
}

void MultiplayerGameState::DisableAllRealtimeActions()
{
	m_active_state = false;
	for (sf::Int32 identifier : m_local_player_identifiers)
	{
		m_players[identifier]->DisableAllRealtimeActions();
	}
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
	sf::Int32 player_count;
	packet >> player_count;

	for (sf::Int32 i = 0; i < player_count; ++i)
	{
		sf::Vector2f position;
		sf::Int32 identifier;
		packet >> identifier >> position.x >> position.y;

		Character* character = m_world.GetCharacter(identifier);
		const bool is_local_player = std::find(m_local_player_identifiers.begin(),
		                                       m_local_player_identifiers.end(),
		                                       identifier) !=
			m_local_player_identifiers.
			end();
		if (character && !is_local_player)
		{
			sf::Vector2f interpolated_position = character->getPosition() + (
				position - character->getPosition()) * 0.1f;
			character->setPosition(interpolated_position);
		}
	}
}

void MultiplayerGameState::HandleSelfSpawn(sf::Packet& packet)
{
	sf::Int32 identifier;
	packet >> identifier;

	m_world.AddCharacter(identifier, true);
	m_players[identifier].reset(new Player(&m_socket, identifier, GetContext().m_keys1));
	m_local_player_identifiers.push_back(identifier);
	m_game_started = true;
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
	sf::Int32 identifier;
	packet >> identifier;

	// Utility::Debug(std::to_string(identifier));

	m_world.AddCharacter(identifier);
	m_players[identifier].reset(new Player(&m_socket, identifier, nullptr));
}

void MultiplayerGameState::HandlePlayerDisconnect(sf::Packet& packet)
{
	sf::Int32 identifier;
	packet >> identifier;
	m_world.RemoveCharacter(identifier);
	m_players.erase(identifier);
}

void MultiplayerGameState::HandleInitialState(sf::Packet& packet)
{
	sf::Int32 player_count;
	packet >> player_count;
	for (sf::Int32 i = 0; i < player_count; ++i)
	{
		sf::Int32 identifier;
		sf::Vector2f position;
		packet >> identifier >> position.x >> position.y;

		Character* character = m_world.AddCharacter(identifier);
		character->setPosition(position);

		m_players[identifier].reset(new Player(&m_socket, identifier, nullptr));
	}
}

void MultiplayerGameState::HandleRealtimeChange(sf::Packet& packet)
{
	sf::Int32 identifier;
	sf::Int32 action;
	bool action_enabled;
	packet >> identifier >> action >> action_enabled;

	const auto itr = m_players.find(identifier);
	if (itr != m_players.end())
	{
		itr->second->HandleNetworkRealtimeChange(static_cast<PlayerAction>(action),
		                                         action_enabled);
	}
}

void MultiplayerGameState::HandlePlayerEvent(sf::Packet& packet)
{
	sf::Int32 identifier;
	sf::Int32 action;
	packet >> identifier >> action;

	const auto itr = m_players.find(identifier);
	if (itr != m_players.end())
	{
		itr->second->HandleNetworkEvent(static_cast<PlayerAction>(action),
		                                m_world.GetCommandQueue());
	}
}

void MultiplayerGameState::HandlePacket(sf::Int32 packet_type, sf::Packet& packet)
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
	case server::PacketType::kPlayerRealtimeChange:
		HandleRealtimeChange(packet);
		break;
	case server::PacketType::kMissionSuccess:
		RequestStackPush(StateID::kLevelWin);
		break;
	case server::PacketType::kUpdateClientState:
		HandleClientUpdate(packet);
		break;
	case server::PacketType::kPlayerEvent:
		HandlePlayerEvent(packet);
		break;
	default:
		break;
	}
}
