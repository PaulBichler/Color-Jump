#pragma once
#include <map>
#include <memory>
#include <string>
#include <SFML/Config.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Thread.hpp>
#include <SFML/Network/Packet.hpp>

class GameServer
{
public:
	GameServer();
	~GameServer();

private:
	struct RemotePeer
	{
		RemotePeer();
		sf::TcpSocket m_socket;
		sf::Time m_last_packet_time;
		sf::Int8 m_identifier{};
		bool m_ready;
		bool m_timed_out;
	};

	struct PlayerInfo
	{
		sf::Int8 m_team_id{};
		sf::Vector2f m_position;
		std::string name;
		sf::Int8 m_color{};
	};

	using peer_ptr = std::unique_ptr<RemotePeer>;
	
	void SetListening(bool enable);
	void ExecutionThread();
	void Tick() const;
	void LobbyTick() const;

	sf::Time Now() const;

	void HandleIncomingConnections();
	void HandleDisconnections();
	void HandleIncomingPackets();
	void HandleIncomingPacket(sf::Packet& packet, RemotePeer& receiving_peer, bool& detected_timeout);
	
	void HandleCheckpointReached(sf::Packet& packet) const;
	void HandleTeamChange(sf::Packet& packet);
	void HandlePlatformUpdate(sf::Packet& packet) const;
	void HandlePlayerPositionUpdate(sf::Packet& packet);
	void HandleTeamRespawn(sf::Packet& packet) const;
	void HandlePlayerNameChange(sf::Packet& packet);
	void HandleGoalReached(sf::Packet& packet) const;
	static void HandlePlayerQuit(RemotePeer& receiving_peer, bool& detected_timeout);

	void NotifyPlayerNameChange(sf::Int8 id, const std::string& name) const;
	void NotifyGoalReached(sf::Int8 team_id) const;
	void NotifyPlatformUpdate(sf::Int8 id, sf::Int8 platform_id, sf::Int8 platform_color) const;
	void NotifyTeamRespawn(sf::Int8 team_id) const;
	void NotifyPlayerJoin(sf::Int8 id) const;
	void NotifyTeamChange(sf::Int8 id, sf::Int8 team_id) const;
	void NotifyGameStart();
	void NotifyGameCountdownStart() const;
	void NotifyCheckpointReached(sf::Int8 team_id, sf::Int8 platform_id) const;
	void NotifyLobbyState(sf::TcpSocket& socket);
	void NotifyClientState() const;

	void GetAndSetID(sf::Int8& id);
	void SendPackageToAll(sf::Packet packet) const;

	static void Debug(const std::string& message);

	sf::Thread m_thread;
	sf::Clock m_clock;
	sf::TcpListener m_listener_socket;
	bool m_listening_state;
	sf::Time m_client_timeout;

	sf::Int8 m_max_connected_players;
	sf::Int8 m_player_count;
	
	std::map<sf::Int8, PlayerInfo> m_player_info;

	std::vector<bool> m_ids;

	std::vector<peer_ptr> m_peers;
	bool m_waiting_thread_end;
	bool m_game_started;
};
