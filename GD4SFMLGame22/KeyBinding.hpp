#pragma once
#include <map>
#include <vector>
#include <SFML/Window/Keyboard.hpp>
#include "PlayerAction.hpp"

class KeyBinding
{
public:

	using action = PlayerAction;
	explicit KeyBinding(int control_pre_configuration);
	void AssignKey(action action, sf::Keyboard::Key key);
	sf::Keyboard::Key GetAssignedKey(action action) const;
	bool CheckAction(sf::Keyboard::Key key, action& out) const;
	std::vector<action> GetRealtimeActions() const;
	
private:

	std::map<sf::Keyboard::Key, action> m_key_map;
};

bool IsRealtimeAction(PlayerAction action);
