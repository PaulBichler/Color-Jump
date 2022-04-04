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
	Player(sf::TcpSocket* socket, sf::Int8 identifier, const KeyBinding* binding);
	void HandleEvent(const sf::Event& event, CommandQueue& commands);
	void HandleRealtimeInput(CommandQueue& commands);
	std::map<PlayerAction, Command>::mapped_type GetAction(PlayerAction action);
	
	bool IsLocal() const;

private:
	void InitialiseActions();

	std::map<PlayerAction, Command> m_action_binding;
	const KeyBinding* m_key_binding;
	std::map<PlayerAction, bool> m_action_proxies;
	sf::Int8 m_identifier;
	sf::TcpSocket* m_socket;
};
