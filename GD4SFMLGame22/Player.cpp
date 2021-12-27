#include "Player.hpp"
#include "Aircraft.hpp"
#include <algorithm>

#include "Character.hpp"


struct CharacterMover
{
	CharacterMover(float vx, float vy) : velocity(vx, vy)
	{
		
	}

	void operator()(Character& character, sf::Time) const
	{
		character.Accelerate(velocity * character.GetMaxSpeed());
	}

	sf::Vector2f velocity;
};

void Player::InitPlayerOne()
{
	m_action_binding[PlayerAction::kMoveLeftOne].action = DerivedAction<Character>(CharacterMover(-1, 0.f));
	m_action_binding[PlayerAction::kMoveLeftOne].category = Category::kPlayerOne;
	m_action_binding[PlayerAction::kMoveRightOne].action = DerivedAction<Character>(CharacterMover(+1, 0.f));
	m_action_binding[PlayerAction::kMoveRightOne].category = Category::kPlayerOne;
	m_action_binding[PlayerAction::kMoveUpOne].action = DerivedAction<Character>(CharacterMover(0, -50.f));
	m_action_binding[PlayerAction::kMoveUpOne].category = Category::kPlayerOne;
}

void Player::InitPlayerTwo()
{
	m_action_binding[PlayerAction::kMoveLeftTwo].action = DerivedAction<Character>(CharacterMover(-1, 0.f));
	m_action_binding[PlayerAction::kMoveLeftTwo].category = Category::kPlayerTwo;
	m_action_binding[PlayerAction::kMoveRightTwo].action = DerivedAction<Character>(CharacterMover(+1, 0.f));
	m_action_binding[PlayerAction::kMoveRightTwo].category = Category::kPlayerTwo;
	m_action_binding[PlayerAction::kMoveUpTwo].action = DerivedAction<Character>(CharacterMover(0, -50.f));
	m_action_binding[PlayerAction::kMoveUpTwo].category = Category::kPlayerTwo;
}

Player::Player()
{
	//Set initial key bindings
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
	if(event.type == sf::Event::KeyPressed)
	{
		auto found = m_key_binding.find(event.key.code);
		if(found != m_key_binding.end() && !IsRealtimeAction(found->second))
		{
			commands.Push(m_action_binding[found->second]);
		}
	}
}

void Player::HandleRealtimeInput(CommandQueue& commands)
{
	//Check if any keybinding keys are pressed
	for(auto pair: m_key_binding)
	{
		if(sf::Keyboard::isKeyPressed(pair.first) && IsRealtimeAction(pair.second))
		{
			commands.Push(m_action_binding[pair.second]);
		}
	}
}

void Player::AssignKey(PlayerAction action, sf::Keyboard::Key key)
{
	//Remove all keys that are already bound to action
	for(auto itr = m_key_binding.begin(); itr != m_key_binding.end();)
	{
		if(itr->second == action)
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

sf::Keyboard::Key Player::GetAssignedKey(PlayerAction action) const
{
	for(auto pair : m_key_binding)
	{
		if(pair.second == action)
		{
			return pair.first;
		}
	}
	return sf::Keyboard::Unknown;
}

void Player::InitialiseActions()
{
	
}

bool Player::IsRealtimeAction(PlayerAction action)
{
	switch(action)
	{
	case PlayerAction::kMoveLeftOne:
	case PlayerAction::kMoveRightOne:
	case PlayerAction::kMoveLeftTwo:
	case PlayerAction::kMoveRightTwo:
		return true;
	default:
		return false;
	}
}
