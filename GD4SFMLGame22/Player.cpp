//Changed by Dylan Goncalves Martins (D00242562)

#include "Player.hpp"

#include <SFML/Network/Packet.hpp>

#include "Character.hpp"
#include "NetworkProtocol.hpp"

//Movement is now for character
struct CharacterMover
{
	CharacterMover(const float vx, const float vy) : velocity(vx, vy)
	{
	}

	void operator()(Character& character, sf::Time) const
	{
		character.Accelerate(character.GetMaxSpeed() * velocity);
	}

	sf::Vector2f velocity;
};

/*
 *	Dylan Goncalves Martins (D00242562)
 *	Initialize player one keys 
 */
void Player::InitPlayer()
{
	m_action_binding[PlayerAction::kMoveLeft].action = DerivedAction<Character>(
		CharacterMover(-1, 0.f));
	m_action_binding[PlayerAction::kMoveRight].action = DerivedAction<Character>(
		CharacterMover(+1, 0.f));
	m_action_binding[PlayerAction::kMoveUp].action = DerivedAction<Character>(
		[](Character& a, sf::Time)
		{
			a.Jump();
		});
}

void Player::HandleRealtimeNetworkInput(CommandQueue& commands)
{
	if (m_socket && !IsLocal())
	{
		// Traverse all realtime input proxies. Because this is a networked game, the input isn't handled directly
		for (auto& pair : m_action_proxies)
		{
			if (pair.second && IsRealtimeAction(pair.first))
				commands.Push(m_action_binding[pair.first]);
		}
	}
}

void Player::HandleNetworkEvent(const PlayerAction action, CommandQueue& commands)
{
	commands.Push(m_action_binding[action]);
}

void Player::HandleNetworkRealtimeChange(const PlayerAction action, const bool action_enabled)
{
	m_action_proxies[action] = action_enabled;
}

void Player::SetMissionStatus(const MissionStatus status)
{
	m_current_mission_status = status;
}

MissionStatus Player::GetMissionStatus() const
{
	return m_current_mission_status;
}

void Player::DisableAllRealtimeActions() const
{
	for (const auto& action : m_action_proxies)
	{
		sf::Packet packet;
		packet << static_cast<sf::Int32>(client::PacketType::kPlayerRealtimeChange);
		packet << m_identifier;
		packet << static_cast<sf::Int32>(action.first);
		packet << false;
		m_socket->send(packet);
	}
}

bool Player::IsLocal() const
{
	// No key binding means this player is remote
	return m_key_binding != nullptr;
}

/*
 *	Dylan Goncalves Martins (D00242562)
 *	Initialize both players keys
 */
Player::Player(sf::TcpSocket* socket, const sf::Int32 identifier, const KeyBinding* binding)
	: m_key_binding(binding)
	  , m_identifier(identifier)
	  , m_socket(socket)
	  , m_current_mission_status(MissionStatus::kMissionRunning)
{
	InitPlayer();

	for (auto& pair : m_action_binding)
		pair.second.category = Category::kPlayer;
}


void Player::HandleEvent(const sf::Event& event, CommandQueue& commands)
{
	// Event
	if (event.type == sf::Event::KeyPressed)
	{
		PlayerAction action;
		if (m_key_binding && m_key_binding->CheckAction(event.key.code, action) && !
			IsRealtimeAction(action))
		{
			// Network connected -> send event over network
			if (m_socket)
			{
				sf::Packet packet;
				packet << static_cast<sf::Int32>(client::PacketType::kPlayerEvent);
				packet << m_identifier;
				packet << static_cast<sf::Int32>(action);
				m_socket->send(packet);
			}

			// Network disconnected -> local event
			else
			{
				commands.Push(m_action_binding[action]);
			}
		}
	}

	// Realtime change (network connected)
	if ((event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) && m_socket)
	{
		PlayerAction action;
		if (m_key_binding && m_key_binding->CheckAction(event.key.code, action) &&
			IsRealtimeAction(action))
		{
			// Send realtime change over network
			sf::Packet packet;
			packet << static_cast<sf::Int32>(client::PacketType::kPlayerRealtimeChange);
			packet << m_identifier;
			packet << static_cast<sf::Int32>(action);
			packet << (event.type == sf::Event::KeyPressed);
			m_socket->send(packet);
		}
	}
}

void Player::HandleRealtimeInput(CommandQueue& commands)
{
	// Check if this is a networked game and local player or just a single player game
	if (m_socket && IsLocal() || !m_socket)
	{
		// Lookup all actions and push corresponding commands to queue
		const std::vector<PlayerAction> active_actions = m_key_binding->GetRealtimeActions();
		for (PlayerAction action : active_actions)
			commands.Push(m_action_binding[action]);
	}
}

sf::Keyboard::Key Player::GetAssignedKey(const PlayerAction action) const
{
	return m_key_binding->GetAssignedKey(action);
}
