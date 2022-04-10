#include "MultiplayerManager.hpp"

#include <utility>

#include "NetworkProtocol.hpp"

//Written by Paul Bichler (D00242563)
void MultiplayerManager::HostServer()
{
	m_game_server = std::make_unique<GameServer>();
}

//Written by Paul Bichler (D00242563)
sf::TcpSocket* MultiplayerManager::ConnectToServer(const sf::IpAddress ip_address, float timeout, bool blocking_mode)
{
	m_socket = std::make_unique<sf::TcpSocket>();
	m_socket->setBlocking(blocking_mode);
	m_socket->connect(ip_address, SERVER_PORT, sf::seconds(timeout));
	return m_socket.get();
}

//Written by Paul Bichler (D00242563)
//Disconnect from the current server and close the server if you are the host
void MultiplayerManager::Disconnect()
{
	m_game_server.reset();
	m_socket.reset();
}

//Written by Paul Bichler (D00242563)
std::vector<std::pair<sf::Int8, sf::Time>>& MultiplayerManager::GetLeaderboard()
{
	return m_leaderboard;
}

//Written by Paul Bichler (D00242563)
//Adds a team to the leaderboard (called when a team finishes the level)
void MultiplayerManager::AddToLeaderboard(sf::Int8 team_id, sf::Time completion_time)
{
	for (const auto& team : m_leaderboard)
		if(team.first == team_id)
			return;

	m_leaderboard.emplace_back(std::pair<sf::Int8, sf::Time>(team_id, completion_time));

	if(m_leaderboard_change_callback)
		m_leaderboard_change_callback();
}

//Written by Paul Bichler (D00242563)
//this callback is invoked when the leaderboard changes (a team is added to the leaderboard for example)
//This is used to update the leaderboard for clients that already finished the game and are in the win/lose state
void MultiplayerManager::SetLeaderboardChangeCallback(std::function<void()> callback)
{
	m_leaderboard_change_callback = std::move(callback);
}

//Written by Paul Bichler (D00242563)
//Adds a player to a team with its name
void MultiplayerManager::AddPlayer(const sf::Int8 team_id, const sf::Int8 id, std::string name)
{
	m_players.try_emplace(id, name);
	m_teams[team_id].emplace_back(id);
}

//Written by Paul Bichler (D00242563)
//Returns the names of the two players a team
std::vector<std::string> MultiplayerManager::GetPlayerNamesOfTeam(const sf::Int8 team_id)
{
	std::vector<std::string> player_names;
	const std::vector<sf::Int8> team = m_teams[team_id];

	player_names.reserve(team.size());
	for ( auto player_id : team)
	{
		player_names.emplace_back(m_players[player_id]);
	}

	return player_names;
}

//Written by Paul Bichler (D00242563)
int MultiplayerManager::GetNumberOfTeams() const
{
	return m_teams.size();
}

//Written by Paul Bichler (D00242563)
//This is used to pass the window focus across states
void MultiplayerManager::SetPassFocus(bool pass_focus)
{
	m_pass_focus = pass_focus;
}

//Written by Paul Bichler (D00242563)
bool MultiplayerManager::GetPassFocus() const
{
	return m_pass_focus;
}

//Written by Paul Bichler (D00242563)
sf::TcpSocket* MultiplayerManager::GetSocket() const
{
	return m_socket.get();
}
