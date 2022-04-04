//Changed by Dylan Goncalves Martins (D00242562)

#include "Player.hpp"
#include "Character.hpp"
#include "NetworkProtocol.hpp"

//Movement is now for character
struct CharacterMover
{
	CharacterMover(const float vx, const float vy) : m_velocity(vx, vy)
	{
	}

	void operator()(Character& character, sf::Time) const
	{
		character.Accelerate(Character::GetMaxSpeed() * m_velocity);
	}

	sf::Vector2f m_velocity;
};

struct CharacterJumpTrigger
{
	explicit CharacterJumpTrigger()
	= default;

	void operator()(Character& character, sf::Time) const
	{
		character.Jump();
	}
};

/*
 *	Dylan Goncalves Martins (D00242562)
 *	Initialize player one keys 
 */
void Player::InitialiseActions()
{
	m_action_binding[PlayerAction::kMoveLeft].action = DerivedAction<Character>(
		CharacterMover(-1, 0.f));
	m_action_binding[PlayerAction::kMoveRight].action = DerivedAction<Character>(
		CharacterMover(+1, 0.f));
	m_action_binding[PlayerAction::kMoveUp].action = DerivedAction<Character>(
		CharacterJumpTrigger());
}

std::map<PlayerAction, Command>::mapped_type Player::GetAction(const PlayerAction action)
{
	return m_action_binding[action];
}

bool Player::IsLocal() const
{
	// No key binding means this player is remote
	return m_key_binding != nullptr;
}

Player::Player(sf::TcpSocket* socket, const sf::Int8 identifier, const KeyBinding* binding)
	: m_key_binding(binding)
	  , m_identifier(identifier)
	  , m_socket(socket)
{
	InitialiseActions();

	for (auto& pair : m_action_binding)
		pair.second.category = Category::kPlayer;
}


void Player::HandleEvent(const sf::Event& event, CommandQueue& commands)
{
	if (event.type == sf::Event::KeyPressed)
	{
		PlayerAction action;
		if (m_key_binding && m_key_binding->CheckAction(event.key.code, action) && !
			IsRealtimeAction(action))
		{
			commands.Push(GetAction(action));
		}
	}
}

void Player::HandleRealtimeInput(CommandQueue& commands)
{
	if (IsLocal() || !m_socket)
	{
		const std::vector<PlayerAction> active_actions = m_key_binding->GetRealtimeActions();
		for (const PlayerAction action : active_actions)
			commands.Push(GetAction(action));
	}
}
