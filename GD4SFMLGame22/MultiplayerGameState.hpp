#pragma once
#include "State.hpp"
#include "Player.hpp"
#include "GameServer.hpp"
#include "MultiplayerWorld.hpp"

class MultiplayerGameState : public State
{
public:
	MultiplayerGameState(StateStack& stack, Context context);
	void OnStackPopped() override;
	void Draw() override;
	void SendPlatformInfo(sf::Int8 player_id, sf::Int8 platform_id, EPlatformType platform) const;
	bool Update(sf::Time dt) override;
	bool HandleEvent(const sf::Event& event) override;
	void SendTeamDeath(sf::Int8 team_id);
	void SendCheckpointReached(sf::Int8 team_id, sf::Int8 platform_id);
	void SendClientDisconnect(sf::Int8 identifier);
	void SendMission(sf::Int8 player_id);

private:
	void UpdateBroadcastMessage(sf::Time elapsed_time);
	void HandleClientUpdate(sf::Packet& packet);
	void HandleSelfSpawn(sf::Packet& packet);
	void HandleBroadcast(sf::Packet& packet);
	void HandlePlayerConnect(sf::Packet& packet);
	void HandlePlayerDisconnect(sf::Packet& packet);
	void HandleInitialState(sf::Packet& packet);
	void HandleUpdatePlatformColors(sf::Packet& packet);
	void HandleUpdatePlayer(sf::Packet& packet) const;
	void HandleMission(sf::Packet& packet);
	void HandleTeamRespawn(sf::Packet& packet) const;
	void HandleTeamCheckpointSet(sf::Packet& packet);
	void HandlePacket(sf::Int8 packet_type, sf::Packet& packet);

	void SendPlayerName(sf::Int8 identifier, sf::Int8 team_id, const std::string& name) const;

private:
	MultiplayerWorld m_world;

	sf::RenderWindow& m_window;

	using PlayerPtr = std::unique_ptr<Player>;
	std::map<int, PlayerPtr> m_players;

	sf::Int8 m_local_player_identifier;
	sf::Clock m_tick_clock;

	std::vector<std::string> m_broadcasts;
	sf::Text m_broadcast_text;
	sf::Time m_broadcast_elapsed_time;

	sf::Text m_player_invitation_text;
	sf::Time m_player_invitation_time;

	sf::Text m_failed_connection_text;
	sf::Clock m_failed_connection_clock;

	bool m_has_focus;
	sf::Time m_client_timeout;
	sf::Time m_time_since_last_packet;
};
