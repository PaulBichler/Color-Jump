#pragma once
#include "Command.hpp"
#include <SFML/Window/Event.hpp>
#include <map>
#include <SFML/Network/TcpSocket.hpp>

#include "CommandQueue.hpp"
#include "KeyBinding.hpp"
#include "PlayerAction.hpp"

enum class MissionStatus;

class Player
{
public:
	Player(sf::TcpSocket* socket, sf::Int32 identifier, const KeyBinding* binding);
	void HandleEvent(const sf::Event& event, CommandQueue& commands);
	void HandleRealtimeInput(CommandQueue& commands);
	void HandleRealtimeNetworkInput(CommandQueue& commands);
	std::map<PlayerAction, Command>::mapped_type GetAction(PlayerAction action);

	//React to events or realtime state changes received over the network
	void HandleNetworkEvent(PlayerAction action, CommandQueue& commands);
	void HandleNetworkRealtimeChange(PlayerAction action, bool action_enabled);


	void SetMissionStatus(MissionStatus status);
	MissionStatus GetMissionStatus() const;

	void DisableAllRealtimeActions() const;
	bool IsLocal() const;

private:
	void InitialiseActions();

	std::map<PlayerAction, Command> m_action_binding;
	const KeyBinding* m_key_binding;
	std::map<PlayerAction, bool> m_action_proxies;
	int m_identifier;
	sf::TcpSocket* m_socket;
	MissionStatus m_current_mission_status;
};

#pragma once
enum class MissionStatus
{
	kMissionRunning,
	kMissionSuccess,
	kMissionFailure
};
