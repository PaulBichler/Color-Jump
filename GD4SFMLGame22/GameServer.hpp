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
	void NotifyPlayerSpawn(sf::Int32 identifier) const;
	void NotifyPlayerRealtimeChange(sf::Int32 identifier, sf::Int32 action,
	                                bool action_enabled) const;
	void NotifyPlayerEvent(sf::Int32 identifier, sf::Int32 action) const;

private:
	struct RemotePeer
	{
		RemotePeer();
		sf::TcpSocket m_socket;
		sf::Time m_last_packet_time;
		std::vector<sf::Int32> m_identifiers;
		bool m_ready;
		bool m_timed_out;
	};

	struct PlayerInfo
	{
		int m_team_identifier{};
		sf::Vector2f m_position;
		std::map<sf::Int32, bool> m_realtime_actions;
	};

	using peer_ptr = std::unique_ptr<RemotePeer>;

private:
	void SetListening(bool enable);
	void ExecutionThread();
	void Tick() const;
	sf::Time Now() const;

	void HandleIncomingPackets();
	void HandleIncomingPacket(sf::Packet& packet, RemotePeer& receiving_peer,
	                          bool& detected_timeout);

	void HandleIncomingConnections();
	void HandleDisconnections();

	void InformWorldState(sf::TcpSocket& socket);
	void BroadcastMessage(const std::string& message) const;
	void SendToAll(sf::Packet& packet) const;
	void UpdateClientState() const;
private:
	sf::Thread m_thread;
	sf::Clock m_clock;
	sf::TcpListener m_listener_socket;
	bool m_listening_state;
	sf::Time m_client_timeout;

	std::size_t m_max_connected_players;
	std::size_t m_connected_players;

	float m_world_height;
	sf::FloatRect m_battlefield_rect;

	std::size_t m_player_count;
	std::map<sf::Int32, PlayerInfo> m_player_info;

	std::vector<peer_ptr> m_peers;
	sf::Int32 m_identifier_counter;
	bool m_waiting_thread_end;
};
