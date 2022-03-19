#include "GameServer.hpp"
#include "NetworkProtocol.hpp"
#include <SFML/System.hpp>

#include <SFML/Network/Packet.hpp>
#include "Utility.hpp"


//It is essential to set the sockets to non-blocking - m_socket.setBlocking(false)
//otherwise the server will hang waiting to read input from a connection

GameServer::RemotePeer::RemotePeer() : m_ready(false), m_timed_out(false)
{
	m_socket.setBlocking(false);
}

GameServer::GameServer(const sf::Vector2f battlefield_size)
	: m_thread(&GameServer::ExecutionThread, this)
	  , m_listening_state(false)
	  , m_client_timeout(sf::seconds(1.f))
	  , m_max_connected_players(15)
	  , m_connected_players(0)
	  , m_world_height(5000.f)
	  , m_battlefield_rect(0.f, m_world_height - battlefield_size.y, battlefield_size.x,
	                       battlefield_size.y)
	  , m_player_count(0)
	  , m_peers(1)
	  , m_identifier_counter(1)
	  , m_waiting_thread_end(false)
{
	m_listener_socket.setBlocking(false);
	m_peers[0].reset(new RemotePeer());
	m_thread.launch();
}

GameServer::~GameServer()
{
	m_waiting_thread_end = true;
	m_thread.wait();
}

//This is the same as SpawnSelf but indicate that an aircraft from a different client is entering the world

void GameServer::NotifyPlayerSpawn(const sf::Int32 identifier)
{
	sf::Packet packet;
	//First thing for every packet is what type of packet it is
	packet << static_cast<sf::Int32>(server::PacketType::kPlayerConnect);
	packet << identifier << m_player_info[identifier].m_position.x <<
		m_player_info[identifier].m_position.y;
	for (std::size_t i = 0; i < m_connected_players; ++i)
	{
		if (m_peers[i]->m_ready)
		{
			m_peers[i]->m_socket.send(packet);
		}
	}
}

//This is the same as PlayerEvent, but for real-time actions. This means that we are changing an ongoing state to either true or false, so we add a Boolean value to the parameters

void GameServer::NotifyPlayerRealtimeChange(const sf::Int32 identifier, const sf::Int32 action,
                                            const bool action_enabled) const
{
	sf::Packet packet;
	//First thing for every packet is what type of packet it is
	packet << static_cast<sf::Int32>(server::PacketType::kPlayerRealtimeChange);
	packet << identifier;
	packet << action;
	packet << action_enabled;

	for (std::size_t i = 0; i < m_connected_players; ++i)
	{
		if (m_peers[i]->m_ready)
		{
			m_peers[i]->m_socket.send(packet);
		}
	}
}

//This takes two sf::Int32 variables, the aircraft identifier and the action identifier
//as declared in the Player class. This is used to inform all peers that plane X has
//triggered an action

void GameServer::NotifyPlayerEvent(const sf::Int32 identifier, const sf::Int32 action) const
{
	sf::Packet packet;
	//First thing for every packet is what type of packet it is
	packet << static_cast<sf::Int32>(server::PacketType::kPlayerEvent);
	packet << identifier;
	packet << action;

	for (std::size_t i = 0; i < m_connected_players; ++i)
	{
		if (m_peers[i]->m_ready)
		{
			m_peers[i]->m_socket.send(packet);
		}
	}
}

void GameServer::SetListening(const bool enable)
{
	//Check if the server listening socket is already listening
	if (enable)
	{
		if (!m_listening_state)
		{
			m_listening_state = (m_listener_socket.listen(SERVER_PORT) == sf::TcpListener::Done);
		}
	}
	else
	{
		m_listener_socket.close();
		m_listening_state = false;
	}
}


void GameServer::ExecutionThread()
{
	SetListening(true);

	const sf::Time frame_rate = sf::seconds(1.f / 60.f);
	sf::Time frame_time = sf::Time::Zero;
	const sf::Time tick_rate = sf::seconds(1.f / 20.f);
	sf::Time tick_time = sf::Time::Zero;
	sf::Clock frame_clock, tick_clock;

	while (!m_waiting_thread_end)
	{
		HandleIncomingConnections();
		HandleIncomingPackets();

		frame_time += frame_clock.getElapsedTime();
		frame_clock.restart();

		tick_time += tick_clock.getElapsedTime();
		tick_clock.restart();

		//Fixed update step
		while (frame_time >= frame_rate)
		{
			frame_time -= frame_rate;
		}

		//Fixed tick step
		while (tick_time >= tick_rate)
		{
			Tick();
			tick_time -= tick_rate;
		}

		//sleep
		sleep(sf::milliseconds(100));
	}
}

void GameServer::Tick()
{
	UpdateClientState();
}

sf::Time GameServer::Now() const
{
	return m_clock.getElapsedTime();
}

void GameServer::HandleIncomingPackets()
{
	bool detected_timeout = false;

	for (peer_ptr& peer : m_peers)
	{
		if (peer->m_ready)
		{
			sf::Packet packet;
			while (peer->m_socket.receive(packet) == sf::Socket::Done)
			{
				//Interpret the packet and react to it
				HandleIncomingPacket(packet, *peer, detected_timeout);

				peer->m_last_packet_time = Now();
				packet.clear();
			}

			if (Now() > peer->m_last_packet_time + m_client_timeout)
			{
				peer->m_timed_out = true;
				detected_timeout = true;
			}
		}
	}

	if (detected_timeout)
	{
		HandleDisconnections();
	}
}

void GameServer::HandleIncomingPacket(sf::Packet& packet, RemotePeer& receiving_peer,
                                      bool& detected_timeout)
{
	sf::Int32 packet_type;
	packet >> packet_type;

	switch (static_cast<client::PacketType>(packet_type))
	{
	case client::PacketType::kQuit:
		{
			receiving_peer.m_timed_out = true;
			detected_timeout = true;
		}
		break;

	case client::PacketType::kPlayerEvent:
		{
			sf::Int32 identifier;
			sf::Int32 action;
			packet >> identifier >> action;
			NotifyPlayerEvent(identifier, action);
		}
		break;

	case client::PacketType::kPlayerRealtimeChange:
		{
			sf::Int32 identifier;
			sf::Int32 action;
			bool action_enabled;
			packet >> identifier >> action >> action_enabled;
			NotifyPlayerRealtimeChange(identifier, action, action_enabled);
		}
		break;

	case client::PacketType::kPositionUpdate:
		{
			sf::Int32 num_player;
			packet >> num_player;

			for (sf::Int32 i = 0; i < num_player; ++i)
			{
				sf::Int32 identifier;
				sf::Int32 hit_points;
				sf::Vector2f position;
				packet >> identifier >> position.x >> position.y >> hit_points;
				m_player_info[identifier].m_position = position;
				m_player_info[identifier].m_hit_points = hit_points;
			}
		}
		break;
	default:
		break;
	}
}

void GameServer::HandleIncomingConnections()
{
	if (!m_listening_state)
	{
		return;
	}

	if (m_listener_socket.accept(m_peers[m_connected_players]->m_socket) == sf::TcpListener::Done)
	{
		//Order the new client to spawn its player 1
		m_player_info[m_identifier_counter].m_position = sf::Vector2f(
			m_battlefield_rect.width / 2, m_battlefield_rect.top + m_battlefield_rect.height / 2);
		m_player_info[m_identifier_counter].m_hit_points = 100;

		sf::Packet packet;
		packet << static_cast<sf::Int32>(server::PacketType::kSpawnSelf);
		packet << m_identifier_counter;
		packet << m_player_info[m_identifier_counter].m_position.x;
		packet << m_player_info[m_identifier_counter].m_position.y;

		m_peers[m_connected_players]->m_identifiers.emplace_back(
			m_identifier_counter);

		BroadcastMessage("New player");
		InformWorldState(m_peers[m_connected_players]->m_socket);
		NotifyPlayerSpawn(m_identifier_counter++);

		m_peers[m_connected_players]->m_socket.send(packet);
		m_peers[m_connected_players]->m_ready = true;
		m_peers[m_connected_players]->m_last_packet_time = Now();

		m_player_count++;
		m_connected_players++;

		if (m_connected_players >= m_max_connected_players)
		{
			SetListening(false);
		}
		else
		{
			m_peers.emplace_back(std::make_unique<RemotePeer>());
		}
	}
}

void GameServer::HandleDisconnections()
{
	for (auto itr = m_peers.begin(); itr != m_peers.end();)
	{
		if ((*itr)->m_timed_out)
		{
			//Inform everyone of a disconnection, erase
			for (sf::Int32 identifier : (*itr)->m_identifiers)
			{
				SendToAll(
					sf::Packet() << static_cast<sf::Int32>(server::PacketType::kPlayerDisconnect)
					<<
					identifier);
				m_player_info.erase(identifier);
			}

			m_connected_players--;
			m_player_count -= (*itr)->m_identifiers.size();

			itr = m_peers.erase(itr);

			//If the number of peers has dropped below max_connections
			if (m_connected_players < m_max_connected_players)
			{
				m_peers.emplace_back(std::make_unique<RemotePeer>());
				SetListening(true);
			}

			BroadcastMessage("A player has disconnected");
		}
		else
		{
			++itr;
		}
	}
}

void GameServer::InformWorldState(sf::TcpSocket& socket)
{
	sf::Packet packet;
	packet << static_cast<sf::Int32>(server::PacketType::kInitialState);
	packet << static_cast<sf::Int32>(m_player_count);

	for (std::size_t i = 0; i < m_connected_players; ++i)
	{
		if (m_peers[i]->m_ready)
		{
			for (sf::Int32 identifier : m_peers[i]->m_identifiers)
			{
				const PlayerInfo player_info = m_player_info[identifier];

				packet << identifier << player_info.m_position.x << player_info.m_position.y <<
					player_info.m_hit_points;
			}
		}
	}

	socket.send(packet);
}

void GameServer::BroadcastMessage(const std::string& message) const
{
	sf::Packet packet;
	packet << static_cast<sf::Int32>(server::PacketType::kBroadcastMessage);
	packet << message;
	for (std::size_t i = 0; i < m_connected_players; ++i)
	{
		if (m_peers[i]->m_ready)
		{
			m_peers[i]->m_socket.send(packet);
		}
	}
}

void GameServer::SendToAll(sf::Packet& packet) const
{
	for (const peer_ptr& peer : m_peers)
	{
		if (peer->m_ready)
		{
			peer->m_socket.send(packet);
		}
	}
}

void GameServer::UpdateClientState() const
{
	sf::Packet packet;
	packet << static_cast<sf::Int32>(server::PacketType::kUpdateClientState);
	packet << static_cast<sf::Int32>(m_player_count);

	for (const auto& player : m_player_info)
	{
		const auto& player_info = player.second;
		packet << player.first << player_info.m_position.x << player_info.m_position.y <<
			player_info.m_hit_points;
	}

	SendToAll(packet);
}