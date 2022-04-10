#include "MultiplayerManager.hpp"

#include <utility>

#include "NetworkProtocol.hpp"

void MultiplayerManager::HostServer()
{
	m_game_server = std::make_unique<GameServer>();
}

sf::TcpSocket* MultiplayerManager::ConnectToServer(const sf::IpAddress ip_address, float timeout, bool blocking_mode)
{
	m_socket = std::make_unique<sf::TcpSocket>();
	m_socket->setBlocking(blocking_mode);
	m_socket->connect(ip_address, SERVER_PORT, sf::seconds(timeout));
	return m_socket.get();
}

void MultiplayerManager::Disconnect()
{
	m_game_server.reset();
	m_socket.reset();
}

std::vector<std::pair<sf::Int8, sf::Time>>& MultiplayerManager::GetLeaderboard()
{
	return m_leaderboard;
}

void MultiplayerManager::AddToLeaderboard(sf::Int8 team_id, sf::Time completion_time)
{
	for (const auto& team : m_leaderboard)
		if(team.first == team_id)
			return;

	m_leaderboard.emplace_back(std::pair<sf::Int8, sf::Time>(team_id, completion_time));

	if(m_leaderboard_change_callback)
		m_leaderboard_change_callback();
}

void MultiplayerManager::SetLeaderboardChangeCallback(std::function<void()> callback)
{
	m_leaderboard_change_callback = std::move(callback);
}

void MultiplayerManager::AddPlayer(const sf::Int8 team_id, const sf::Int8 id, std::string name)
{
	m_players.try_emplace(id, name);
	m_teams[team_id].emplace_back(id);
}

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

int MultiplayerManager::GetNumberOfTeams() const
{
	return m_teams.size();
}

sf::TcpSocket* MultiplayerManager::GetSocket() const
{
	return m_socket.get();
}
