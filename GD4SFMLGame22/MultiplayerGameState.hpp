#pragma once
#include "State.hpp"
#include "Player.hpp"
#include "GameServer.hpp"
#include "MultiplayerWorld.hpp"
#include "NetworkProtocol.hpp"

class MultiplayerGameState : public State
{
public:
	MultiplayerGameState(StateStack& stack, Context context, bool is_host);
	void Draw() override;
	void SendPlatformInfo(sf::Int32 team_id, const sf::Int32 platform_id, EPlatformType platform);
	bool Update(sf::Time dt) override;
	bool HandleEvent(const sf::Event& event) override;
	virtual void OnActivate();
	void OnDestroy();
	void DisableAllRealtimeActions();

private:
	void UpdateBroadcastMessage(sf::Time elapsed_time);
	void HandleClientUpdate(sf::Packet& packet);
	void HandleSelfSpawn(sf::Packet& packet);
	void HandleBroadcast(sf::Packet& packet);
	void HandlePlayerConnect(sf::Packet& packet);
	void HandlePlayerDisconnect(sf::Packet& packet);
	void HandleInitialState(sf::Packet& packet);
	void HandleRealtimeChange(sf::Packet& packet);
	void HandlePlayerEvent(sf::Packet& packet);
	void HandleTeamSelection(sf::Packet& packet) const;
	void HandleUpdatePlatformColors(sf::Packet& packet) const;
	void HandleUpdatePlayerName(sf::Packet& packet) const;
	void HandlePacket(sf::Int32 packet_type, sf::Packet& packet);

	void SendPlayerName(sf::Int32 identifier, const std::string& name);

private:
	MultiplayerWorld m_world;

	sf::RenderWindow& m_window;
	TextureHolder& m_texture_holder;

	typedef std::unique_ptr<Player> PlayerPtr;
	std::map<int, PlayerPtr> m_players;
	
	std::vector<sf::Int32> m_local_player_identifiers;
	sf::TcpSocket m_socket;
	bool m_connected;
	std::unique_ptr<GameServer> m_game_server;
	sf::Clock m_tick_clock;

	std::vector<std::string> m_broadcasts;
	sf::Text m_broadcast_text;
	sf::Time m_broadcast_elapsed_time;

	sf::Text m_player_invitation_text;
	sf::Time m_player_invitation_time;

	sf::Text m_failed_connection_text;
	sf::Clock m_failed_connection_clock;

	bool m_active_state;
	bool m_has_focus;
	bool m_host;
	bool m_game_started;
	sf::Time m_client_timeout;
	sf::Time m_time_since_last_packet;
};

