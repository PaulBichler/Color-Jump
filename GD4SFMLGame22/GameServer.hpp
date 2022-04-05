#pragma once
#include <map>
#include <memory>
#include <string>
#include <SFML/Config.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Thread.hpp>

class GameServer
{
public:
	explicit GameServer(sf::Vector2f battlefield_size);
	~GameServer();
	void SendPackageToAll(sf::Packet packet) const;
	void NotifyPlayerSpawn(sf::Int8 id) const;
	void NotifyPlayerNameChange(sf::Int8 identifier, const std::string& name) const;
	void NotifyTeamRespawn(sf::Int8 team_id) const;

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
		std::map<sf::Int8, sf::Int8> m_platform_colors;
		sf::Int8 m_color;
	};

	using peer_ptr = std::unique_ptr<RemotePeer>;

private:
	void SetListening(bool enable);
	void LobbyTick() const;
	void ExecutionThread();
	void Tick() const;
	sf::Time Now() const;

	void HandleIncomingPackets();
	void NotifyPlayerPlatformChange(sf::Int8 player_id, sf::Int8 platform_id, sf::Int8 platform_color) const;
	void NotifyMission(sf::Int8 team_id) const;
	void NotifyTeamCheckpointSet(sf::Int8 team_id, sf::Int8 platform_id) const;
	void NotifyTeamChange(const sf::Int8 identifier, const sf::Int8 team_id, const sf::Int8 color);
	void NotifyGameStart();
	void HandleIncomingPacket(sf::Packet& packet, RemotePeer& receiving_peer, bool& detected_timeout);
	void CreateSpawnSelfPacket(sf::Packet& packet, sf::Int8 id);

	void HandleIncomingConnections();
	void HandleDisconnections();

	void InformWorldState(sf::TcpSocket& socket);
	void SendToAll(sf::Packet& packet) const;
	void UpdateClientState() const;
	static void Debug(const std::string& message);

private:
	sf::Thread m_thread;
	sf::Clock m_clock;
	sf::TcpListener m_listener_socket;
	bool m_listening_state;
	sf::Time m_client_timeout;

	sf::Int8 m_max_connected_players;
	sf::Int8 m_connected_players;

	float m_world_height;
	sf::FloatRect m_battlefield_rect;

	sf::Int8 m_player_count;
	std::map<sf::Int8, PlayerInfo> m_player_info;

	std::vector<bool> m_ids;

	std::vector<peer_ptr> m_peers;
	bool m_waiting_thread_end;
	bool m_game_started;
};
