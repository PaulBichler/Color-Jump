#include "GameServer.hpp"
#include "NetworkProtocol.hpp"
#include <SFML/System.hpp>
#include "Utility.hpp"

GameServer::RemotePeer::RemotePeer() : m_ready(false), m_timed_out(false)
{
	m_socket.setBlocking(false);
}

GameServer::GameServer()
	: m_thread(&GameServer::ExecutionThread, this)
	  , m_listening_state(false)
	  , m_client_timeout(sf::seconds(2.f))
	  , m_max_connected_players(16)
	  , m_player_count(0)
	  , m_ids(16)
	  , m_peers(1)
	  , m_waiting_thread_end(false)
	  , m_game_started(false)
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

void GameServer::SendPackageToAll(sf::Packet packet) const
{
	for (sf::Int8 i = 0; i < m_player_count; ++i)
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
			m_listening_state = m_listener_socket.listen(SERVER_PORT) == sf::TcpListener::Done;
		}
	}
	else
	{
		m_listener_socket.close();
		m_listening_state = false;
	}
}

/**
 * Dylan Goncalves Martins (D00242562)
 * \brief Every half a second send package to clients so they know that they didn't lose connection
 */
void GameServer::LobbyTick() const
{
	sf::Packet packet;
	packet << static_cast<sf::Int8>(server::PacketType::kLobbyUpdate);

	SendPackageToAll(packet);
}

void GameServer::ExecutionThread()
{
	SetListening(true);

	const sf::Time frame_rate = sf::seconds(1.f / 60.f);
	sf::Time frame_time = sf::Time::Zero;
	const sf::Time game_tick_rate = sf::seconds(1.f / 24.f);
	sf::Time game_tick_time = sf::Time::Zero;
	const sf::Time lobby_tick_rate = sf::seconds(1.f / 2.f);
	sf::Time lobby_tick_time = sf::Time::Zero;
	sf::Clock frame_clock, game_tick_clock, lobby_tick_clock;

	while (!m_waiting_thread_end)
	{
		HandleIncomingConnections();
		HandleIncomingPackets();

		frame_time += frame_clock.getElapsedTime();
		frame_clock.restart();

		game_tick_time += game_tick_clock.getElapsedTime();
		game_tick_clock.restart();

		lobby_tick_time += lobby_tick_clock.getElapsedTime();
		lobby_tick_clock.restart();

		//Fixed update step
		while (frame_time >= frame_rate)
		{
			frame_time -= frame_rate;
		}

		//Fixed tick step
		while (game_tick_time >= game_tick_rate)
		{
			if (m_game_started)
			{
				Tick();
			}
			game_tick_time -= game_tick_rate;
		}

		while (lobby_tick_time >= lobby_tick_rate)
		{
			if (!m_game_started)
			{
				LobbyTick();
			}
			lobby_tick_time -= lobby_tick_rate;
		}

		//sleep
		sleep(sf::milliseconds(100));
	}
}

void GameServer::Tick() const
{
	NotifyClientState();
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

/**
 * Dylan Goncalves Martins (D00242562)
 * \brief Send to every one the start packet and then foreach client a spawn self packet
 */
void GameServer::NotifyGameStart()
{
	m_game_started = true;
	sf::Packet packet;
	packet << static_cast<sf::Int8>(server::PacketType::kStartGame);
	SendPackageToAll(packet);

	for (int i = 0; i < m_player_count; ++i)
	{
		if (m_peers[i]->m_ready)
		{
			packet.clear();

			packet << static_cast<sf::Int8>(server::PacketType::kSpawnSelf);

			packet << static_cast<sf::Int8>(m_player_info.size() - 1);

			sf::Int8 id = m_peers[i]->m_identifier;

			for (const auto& player_info : m_player_info)
			{
				if (player_info.first == id)
				{
					continue;
				}

				packet << player_info.first;
				packet << player_info.second.m_team_id;
				packet << player_info.second.m_color;
				packet << player_info.second.name;
			}

			packet << id;
			packet << m_player_info[id].m_team_id;
			packet << m_player_info[id].m_color;
			packet << m_player_info[id].name;

			m_peers[i]->m_socket.send(packet);
		}
	}

	Debug("Start game on all sockets");
	SetListening(false);
}

void GameServer::NotifyGameCountdownStart() const
{
	sf::Packet packet;
	packet << static_cast<sf::Int8>(server::PacketType::kStartGameCountdown);
	SendPackageToAll(packet);
}

void GameServer::NotifyCheckpointReached(const sf::Int8 team_id, const sf::Int8 platform_id) const
{
	sf::Packet notify_packet;
	notify_packet << static_cast<sf::Int8>(server::PacketType::kSetTeamCheckpoint);
	notify_packet << team_id;
	notify_packet << platform_id;

	Debug("New checkpoint set for team: " + std::to_string(team_id));

	SendPackageToAll(notify_packet);
}

void GameServer::HandleCheckpointReached(sf::Packet& packet) const
{
	sf::Int8 team_id;
	sf::Int8 platform_id;
	packet >> team_id;
	packet >> platform_id;

	NotifyCheckpointReached(team_id, platform_id);
}

/**
 * Dylan Goncalves Martins (D00242562)
 * \brief package with the new player id send to everyone
 * \param id player id
 */
void GameServer::NotifyPlayerJoin(const sf::Int8 id) const
{
	sf::Packet packet;
	packet << static_cast<sf::Int8>(server::PacketType::kPlayerConnect);
	packet << id;

	Debug("Player " + std::to_string(id) + " joined.");

	SendPackageToAll(packet);
}

/**
 * Dylan Goncalves Martins (D00242562)
 * \brief Notifies all clients of a team change for a player
 * \param id player id
 * \param team_id new team id
 */
void GameServer::NotifyTeamChange(const sf::Int8 id, const sf::Int8 team_id) const
{
	sf::Packet notify_packet;
	notify_packet << static_cast<sf::Int8>(server::PacketType::kTeamSelection);
	notify_packet << id;
	notify_packet << team_id;

	Debug("Team change");

	SendPackageToAll(notify_packet);
}

/**
 * Dylan Goncalves Martins (D00242562)
 * \brief reads the packet and set the player info for the incoming id (team id and player color)
 * \param packet incoming packet
 */
void GameServer::HandleTeamChange(sf::Packet& packet)
{
	sf::Int8 id;
	sf::Int8 team_id;
	sf::Int8 color;
	packet >> id >> team_id >> color;

	m_player_info[id].m_team_id = team_id;
	m_player_info[id].m_color = color;

	NotifyTeamChange(id, team_id);
}

/**
 * \brief sends a packet with the team id in it to all clients, informing of a team respawn
 * \param team_id team id
 */
void GameServer::NotifyTeamRespawn(const sf::Int8 team_id) const
{
	sf::Packet notify_packet;
	notify_packet << static_cast<sf::Int8>(server::PacketType::kRespawnTeam);
	notify_packet << team_id;

	Debug("Respawn team " + std::to_string(team_id));

	SendPackageToAll(notify_packet);
}

/**
 * \brief just reads in the team id, and then notifies all clients
 * \param packet packet
 */
void GameServer::HandleTeamRespawn(sf::Packet& packet) const
{
	sf::Int8 team_id;
	packet >> team_id;

	NotifyTeamRespawn(team_id);
}

/**
 * \brief Notifies all clients of a name change
 * \param id player id
 * \param name chosen name
 */
void GameServer::NotifyPlayerNameChange(const sf::Int8 id, const std::string& name) const
{
	sf::Packet notify_packet;
	notify_packet << static_cast<sf::Int8>(server::PacketType::kUpdatePlayer);
	notify_packet << id;
	notify_packet << name;

	Debug("Update player info.");

	SendPackageToAll(notify_packet);
}

/**
 * \brief Reads packet and stores name in player info
 * also only takes a max of 20 chars
 * \param packet packet
 */
void GameServer::HandlePlayerNameChange(sf::Packet& packet)
{
	sf::Int8 id;
	std::string name;
	packet >> id >> name;
	name = name.substr(0, 20);
	m_player_info[id].name = name;

	NotifyPlayerNameChange(id, name);
}

/**
 * \brief Sends the team id of the team who reached the goal tile to all clients 
 * \param team_id team id
 */
void GameServer::NotifyGoalReached(const sf::Int8 team_id) const
{
	sf::Packet notify_packet;
	notify_packet << static_cast<sf::Int8>(server::PacketType::kMissionSuccess);
	notify_packet << team_id;

	Debug("Mission completed");

	SendPackageToAll(notify_packet);
}

/**
 * \brief just reads in the team id, and then notifies all clients
 * \param packet packet
 */
void GameServer::HandleGoalReached(sf::Packet& packet) const
{
	sf::Int8 team_id;
	packet >> team_id;

	NotifyGoalReached(team_id);
}

/**
 * Dylan Goncalves Martins (D00242562)
 * \brief Sends the platform changes to all clients
 * \param id player id
 * \param platform_id platform id
 * \param platform_color platform color
 */
void GameServer::NotifyPlatformUpdate(const sf::Int8 id, const sf::Int8 platform_id, const sf::Int8 platform_color) const
{
	sf::Packet packet;
	packet << static_cast<sf::Int8>(server::PacketType::kUpdatePlatformColors);
	packet << id;
	packet << platform_id;
	packet << platform_color;

	Debug("Platform changes for " + std::to_string(id));

	SendPackageToAll(packet);
}

/**
 * Dylan Goncalves Martins (D00242562)
 * \brief Handles incoming platform changes
 * \param packet packet
 */
void GameServer::HandlePlatformUpdate(sf::Packet& packet) const
{
	sf::Int8 id;
	sf::Int8 platform_id;
	sf::Int8 platform_color;

	packet >> id >> platform_id >> platform_color;

	NotifyPlatformUpdate(id, platform_id, platform_color);
}

/**
 * Dylan Goncalves Martins (D00242562)
 * \brief Updates server saved positions
 * \param packet packet
 */
void GameServer::HandlePlayerPositionUpdate(sf::Packet& packet)
{
	sf::Int8 id;
	sf::Vector2f position;
	packet >> id >> position.x >> position.y;
	m_player_info[id].m_position = position;
}

/**
 * \brief Sets a player who wants to quit to timeout
 */
void GameServer::HandlePlayerQuit(RemotePeer& receiving_peer, bool& detected_timeout)
{
	receiving_peer.m_timed_out = true;
	detected_timeout = true;
}

void GameServer::HandleIncomingPacket(sf::Packet& packet, RemotePeer& receiving_peer, bool& detected_timeout)
{
	sf::Int8 packet_type;
	packet >> packet_type;

	switch (static_cast<client::PacketType>(packet_type))
	{
	case client::PacketType::kQuit:
		HandlePlayerQuit(receiving_peer, detected_timeout);
		break;
	case client::PacketType::kPositionUpdate:
		HandlePlayerPositionUpdate(packet);
		break;
	case client::PacketType::kPlatformUpdate:
		HandlePlatformUpdate(packet);
		break;
	case client::PacketType::kPlayerUpdate:
		HandlePlayerNameChange(packet);
		break;
	case client::PacketType::kGoalReached:
		HandleGoalReached(packet);
		break;
	case client::PacketType::kTeamDeath:
		HandleTeamRespawn(packet);
		break;
	case client::PacketType::kTeamChange:
		HandleTeamChange(packet);
		break;
	case client::PacketType::kCheckpointReached:
		HandleCheckpointReached(packet);
		break;
	case client::PacketType::kStartNetworkGame:
		NotifyGameStart();
		break;
	case client::PacketType::kStartNetworkGameCountdown:
		NotifyGameCountdownStart();
		break;
	default:
		break;
	}
}

/**
 * Dylan Goncalves Martins (D00242562)
 * \brief This method finds the first available id and sets it to being used, then it return the id
 * \param id player id
 */
void GameServer::GetAndSetID(sf::Int8& id)
{
	for (sf::Int8 i = 0; i < static_cast<sf::Int8>(m_ids.size()); ++i)
	{
		if (m_ids[i])
			continue;

		id = i;
		m_ids[i] = true;
		break;
	}
}

void GameServer::HandleIncomingConnections()
{
	if (!m_listening_state)
	{
		return;
	}

	if (m_listener_socket.accept(m_peers[m_player_count]->m_socket) == sf::TcpListener::Done)
	{
		if (m_player_count >= m_max_connected_players)
		{
			SetListening(false);
			return;
		}


		sf::Int8 id = 0;

		GetAndSetID(id);

		id += 1;

		//Order the new client to spawn its player 1
		m_player_info[id].m_position = sf::Vector2f(0, 0);

		sf::Packet packet;
		packet << static_cast<sf::Int8>(server::PacketType::kSpawnSelf);
		packet << id;

		m_peers[m_player_count]->m_identifier = id;

		NotifyLobbyState(m_peers[m_player_count]->m_socket);
		NotifyPlayerJoin(id);

		m_peers[m_player_count]->m_socket.send(packet);

		Debug("Send self spawn package to " + std::to_string(id) + ".");

		m_peers[m_player_count]->m_ready = true;
		m_peers[m_player_count]->m_last_packet_time = Now();

		m_player_count++;
		m_peers.emplace_back(std::make_unique<RemotePeer>());
	}
}

void GameServer::HandleDisconnections()
{
	for (auto itr = m_peers.begin(); itr != m_peers.end();)
	{
		if ((*itr)->m_timed_out)
		{
			const auto& disconnect_packet = sf::Packet() << static_cast<sf::Int8>(server::PacketType::kPlayerDisconnect)
				<< (*itr)->m_identifier;
			SendPackageToAll(disconnect_packet);
			m_ids[(*itr)->m_identifier - 1] = false;
			m_player_info.erase((*itr)->m_identifier);

			m_player_count--;

			itr = m_peers.erase(itr);

			//If the number of peers has dropped below max_connections (and the game hasn't started yet)
			if (m_player_count < m_max_connected_players && !m_game_started)
			{
				m_peers.emplace_back(std::make_unique<RemotePeer>());
				SetListening(true);
			}
		}
		else
		{
			++itr;
		}
	}
}

/**
 * Dylan Goncalves Martins (D00242562)
 * \brief Sends an Initial Lobby state to the person joining
 * \param socket receiver
 */
void GameServer::NotifyLobbyState(sf::TcpSocket& socket)
{
	sf::Packet packet;
	packet << static_cast<sf::Int8>(server::PacketType::kInitialState);
	packet << m_player_count;

	for (sf::Int8 i = 0; i < m_player_count; ++i)
	{
		if (m_peers[i]->m_ready)
		{
			sf::Int8 identifier = m_peers[i]->m_identifier;
			const PlayerInfo player_info = m_player_info[identifier];
			packet << identifier << player_info.m_team_id << player_info.name;
		}
	}

	Debug("Inform world state to connecting socket.");

	socket.send(packet);
}

/**
 * Dylan Goncalves Martins (D00242562)
 * \brief Notifies all client positions 
 */
void GameServer::NotifyClientState() const
{
	sf::Packet packet;
	packet << static_cast<sf::Int8>(server::PacketType::kUpdateClientState);
	packet << m_player_count;

	for (const auto& player : m_player_info)
	{
		const auto& player_info = player.second;
		packet << player.first << player_info.m_position.x << player_info.m_position.y;
	}

	SendPackageToAll(packet);
}

void GameServer::Debug(const std::string& message)
{
	Utility::Debug("Server: " + message);
}
