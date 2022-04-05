#pragma once
#include "Container.hpp"
#include "Label.hpp"
#include "State.hpp"
#include "Player.hpp"
#include "MultiplayerWorld.hpp"

class MultiplayerGameState : public State
{
public:
	MultiplayerGameState(StateStack& stack, Context& context);
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
	void HandleClientUpdate(sf::Packet& packet);
	static void Debug(const std::string& message);
	void HandleSelfSpawn(sf::Packet& packet);
	void HandlePlayerConnect(sf::Packet& packet);
	void HandlePlayerDisconnect(sf::Packet& packet);
	void HandleUpdatePlatformColors(sf::Packet& packet);
	void HandleUpdatePlayer(sf::Packet& packet) const;
	void HandleMission(sf::Packet& packet) const;
	void HandleTeamRespawn(sf::Packet& packet) const;
	void HandleTeamCheckpointSet(sf::Packet& packet);
	void HandlePacket(sf::Int8 packet_type, sf::Packet& packet);

	void SendPlayerName(sf::Int8 identifier, const std::string& name) const;

private:
	MultiplayerWorld m_world;
	bool m_connected;

	using PlayerPtr = std::unique_ptr<Player>;
	std::map<int, PlayerPtr> m_players;

	sf::Int8 m_local_player_identifier;
	sf::Clock m_tick_clock;

	GUI::Label::Ptr m_failed_connection_text;
	GUI::Container m_gui_fail_container;
	sf::Clock m_failed_connection_clock;

	bool m_has_focus;
	sf::Time m_client_timeout;
	sf::Time m_time_since_last_packet;
};
