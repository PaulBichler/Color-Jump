#pragma once
#include <string>

#include "KeyBinding.hpp"

class PlayerDataManager
{
public:
	PlayerDataManager();

	struct PlayerData
	{
		PlayerData() : m_player_name("Handsome Player"), m_ip_address("127.0.0.1"), m_player1_keybindings(1), m_player2_keybindings(2) { }
		std::string m_player_name;
		std::string m_ip_address;
		KeyBinding m_player1_keybindings;
		KeyBinding m_player2_keybindings;
	};

	void Save() const;
	void Load();
	PlayerData& GetData();

private:
	PlayerData m_player_data;
};

