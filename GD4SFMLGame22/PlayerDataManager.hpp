#pragma once
#include <string>

#include "KeyBinding.hpp"

class PlayerDataManager
{
public:
	PlayerDataManager();

	struct PlayerData
	{
		PlayerData() : m_player_name("Player"), m_player1_keybindings(1), m_player2_keybindings(2) { }
		std::string m_player_name;
		KeyBinding m_player1_keybindings;
		KeyBinding m_player2_keybindings;
	};

	void Save() const;
	void Load();
	PlayerData& GetData();

private:
	PlayerData m_player_data;
};

