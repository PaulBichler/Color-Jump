#include "PlayerDataManager.hpp"
#include <fstream>
#include <sstream>

//Written by Paul Bichler (D00242563)
PlayerDataManager::PlayerDataManager()
{
	Load();
}

//Written by Paul Bichler (D00242563)
//Saves the player configuration to a file.
//Data saved are: Player Name, Ip Address used to join a server, key bindings of player 1 and 2
void PlayerDataManager::Save() const
{
	std::ofstream out("PlayerData/player_data.txt");

	out << m_player_data.m_player_name
		<< "\n"
		<< m_player_data.m_ip_address
		<< "\n"
		<< std::to_string(static_cast<int>(PlayerAction::kMoveLeft)) << "," << std::to_string(m_player_data.m_player1_keybindings.GetAssignedKey(PlayerAction::kMoveLeft)) << ";"
		<< std::to_string(static_cast<int>(PlayerAction::kMoveRight)) << "," << std::to_string(m_player_data.m_player1_keybindings.GetAssignedKey(PlayerAction::kMoveRight)) << ";"
		<< std::to_string(static_cast<int>(PlayerAction::kMoveUp)) << "," << std::to_string(m_player_data.m_player1_keybindings.GetAssignedKey(PlayerAction::kMoveUp)) << ";"
		<< "\n"
		<< std::to_string(static_cast<int>(PlayerAction::kMoveLeft)) << "," << std::to_string(m_player_data.m_player2_keybindings.GetAssignedKey(PlayerAction::kMoveLeft)) << ";"
		<< std::to_string(static_cast<int>(PlayerAction::kMoveRight)) << "," << std::to_string(m_player_data.m_player2_keybindings.GetAssignedKey(PlayerAction::kMoveRight)) << ";"
		<< std::to_string(static_cast<int>(PlayerAction::kMoveUp)) << "," << std::to_string(m_player_data.m_player2_keybindings.GetAssignedKey(PlayerAction::kMoveUp)) << ";"
		<< "\n";

	out.close();
}

//Written by Paul Bichler (D00242563)
//Load the player configuration from the save file.
void PlayerDataManager::Load()
{
	std::ifstream in("PlayerData/player_data.txt");

	if(in.fail())
	{
		//file was not found, do nothing (uses default values).
		return;
	}

	std::string line;

	//Load Name
	std::getline(in, line);
	m_player_data.m_player_name = line.substr(0, 15.f);

	//Load Ip Address
	std::getline(in, line);
	m_player_data.m_ip_address = line.substr(0, 25.f);

	//Load Key Bindings
	for(int i = 0; i < 2; i++)
	{
		std::getline(in, line);
		std::stringstream ss(line);
		std::string token;

		while (std::getline(ss, token, ';'))
		{
			std::stringstream ss2(token);
			std::string token2;

			std::getline(ss2, token2, ',');
			auto action = static_cast<PlayerAction>(std::stoi(token2));
			std::getline(ss2, token2, ',');
			auto key = static_cast<sf::Keyboard::Key>(std::stoi(token2));

			if(i == 0)
				m_player_data.m_player1_keybindings.AssignKey(action, key);
			else
				m_player_data.m_player2_keybindings.AssignKey(action, key);
		}
	}

	in.close();
}

//Written by Paul Bichler (D00242563)
PlayerDataManager::PlayerData& PlayerDataManager::GetData()
{
	return m_player_data;
}
