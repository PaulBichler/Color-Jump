//Changed by Dylan Goncalves Martins (D00242562)

#include "Player.hpp"
#include "Character.hpp"

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
void Player::InitPlayerOne()
{
	m_action_binding[PlayerAction::kMoveLeftOne].action = DerivedAction<Character>(CharacterMover(-1, 0.f));
	m_action_binding[PlayerAction::kMoveLeftOne].category = Category::kPlayerOne;
	m_action_binding[PlayerAction::kMoveRightOne].action = DerivedAction<Character>(CharacterMover(+1, 0.f));
	m_action_binding[PlayerAction::kMoveRightOne].category = Category::kPlayerOne;
	m_action_binding[PlayerAction::kMoveUpOne].category = Category::kPlayerOne;

	m_action_binding[PlayerAction::kMoveUpOne].action = DerivedAction<Character>([](Character& a, sf::Time)
	{
		a.Jump();
	});
	m_action_binding[PlayerAction::kMoveUpOne].category = Category::kPlayerOne;
}


/*
 *	Dylan Goncalves Martins (D00242562)
 *	Initialize player two keys
 */
void Player::InitPlayerTwo()
{
	m_action_binding[PlayerAction::kMoveLeftTwo].action = DerivedAction<Character>(CharacterMover(-1, 0.f));
	m_action_binding[PlayerAction::kMoveLeftTwo].category = Category::kPlayerTwo;
	m_action_binding[PlayerAction::kMoveRightTwo].action = DerivedAction<Character>(CharacterMover(+1, 0.f));
	m_action_binding[PlayerAction::kMoveRightTwo].category = Category::kPlayerTwo;
	m_action_binding[PlayerAction::kMoveUpTwo].action = DerivedAction<Character>([](Character& a, sf::Time)
	{
		a.Jump();
	});
	m_action_binding[PlayerAction::kMoveUpTwo].category = Category::kPlayerTwo;
}

/*
 *	Dylan Goncalves Martins (D00242562)
 *	Initialize both players keys
 */
Player::Player()
{
	m_key_binding[sf::Keyboard::A] = PlayerAction::kMoveLeftOne;
	m_key_binding[sf::Keyboard::D] = PlayerAction::kMoveRightOne;
	m_key_binding[sf::Keyboard::W] = PlayerAction::kMoveUpOne;

	InitPlayerOne();

	m_key_binding[sf::Keyboard::Left] = PlayerAction::kMoveLeftTwo;
	m_key_binding[sf::Keyboard::Right] = PlayerAction::kMoveRightTwo;
	m_key_binding[sf::Keyboard::Up] = PlayerAction::kMoveUpTwo;

	InitPlayerTwo();
}


void Player::HandleEvent(const sf::Event& event, CommandQueue& commands)
{
	if (event.type == sf::Event::KeyPressed)
	{
		const auto found = m_key_binding.find(event.key.code);
		if (found != m_key_binding.end() && !IsRealtimeAction(found->second))
		{
			commands.Push(m_action_binding[found->second]);
		}
	}
}

void Player::HandleRealtimeInput(CommandQueue& commands) const
{
	//Check if any keyBinding keys are pressed
	for (auto& pair : m_key_binding)
	{
		if (sf::Keyboard::isKeyPressed(pair.first) && IsRealtimeAction(pair.second))
		{
			commands.Push(m_action_binding.at(pair.second));
		}
	}
}

void Player::AssignKey(const PlayerAction action, const sf::Keyboard::Key key)
{
	//Remove all keys that are already bound to action
	for (auto itr = m_key_binding.begin(); itr != m_key_binding.end();)
	{
		if (itr->second == action)
		{
			m_key_binding.erase(itr++);
		}
		else
		{
			++itr;
		}
	}
	m_key_binding[key] = action;
}

sf::Keyboard::Key Player::GetAssignedKey(const PlayerAction action) const
{
	for (const auto& pair : m_key_binding)
	{
		if (pair.second == action)
		{
			return pair.first;
		}
	}
	return sf::Keyboard::Unknown;
}

void Player::InitialiseActions()
{
}

bool Player::IsRealtimeAction(const PlayerAction action)
{
	switch (action)
	{
	case PlayerAction::kMoveLeftOne:
	case PlayerAction::kMoveRightOne:
	case PlayerAction::kMoveLeftTwo:
	case PlayerAction::kMoveRightTwo:
		return true;
	case PlayerAction::kMoveUpOne:
	case PlayerAction::kMoveUpTwo:
	case PlayerAction::kActionCount:
		return false;
	}
	return false;
}
