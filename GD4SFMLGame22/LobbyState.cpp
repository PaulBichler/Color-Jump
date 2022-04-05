#include "LobbyState.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

#include "NetworkProtocol.hpp"
#include "Utility.hpp"
#include <SFML/Network/Packet.hpp>

void LobbyState::CreateUI(const Context context)
{
	int y = context.m_window->getSize().y / 2;
	int x = context.m_window->getSize().x / 2;
	Utility::CreateLabel(context, m_failed_connection_text, x, y, "Attempting to connect...", 35);
	Utility::CentreOrigin(m_failed_connection_text->GetText());
	m_gui_fail_container.Pack(m_failed_connection_text);

	Utility::CreateButton(context, m_change_name_button, 80, 850, "Change Name", true);
	m_gui_container.Pack(m_change_name_button);

	Utility::CreateLabel(context, m_current_name_label, 310, 865, m_player_input_name, 20);
	m_gui_container.Pack(m_current_name_label);

	std::shared_ptr<GUI::Label> unpaired_label;
	Utility::CreateLabel(context, unpaired_label, 80, 190, "Unpaired Players", 25);
	m_gui_container.Pack(unpaired_label);

	std::shared_ptr<GUI::Button> start_game;
	Utility::CreateButton(context, start_game, 600, 850, "Start game", [this]
	{
		sf::Packet packet;
		packet << static_cast<sf::Int8>(client::PacketType::kStartNetworkGame);
		m_context.m_socket->send(packet);
	}, [this]
	{
		return m_is_host;
	});
	m_gui_container.Pack(start_game);

	std::shared_ptr<GUI::Button> back_button;
	Utility::CreateButton(context, back_button, 1080, 850, "Back", [this]
	{
		RequestStackPop();
		RequestStackPush(StateID::kMenu);
	});
	m_gui_container.Pack(back_button);

	for (int i = 1; i <= 8; ++i)
	{
		std::shared_ptr<GUI::Button> team_button;
		y = 150 + 150 * ((i - 1 - (i - 1) % 2) / 2);
		x = i % 2 == 0 ? 900 : 500;
		auto label = "Team " + std::to_string(i);
		Utility::CreateButton(context, back_button, x, y, label, [this, i]
		{
			HandleTeamChoice(i);
		});
		m_gui_container.Pack(back_button);
	}
}

LobbyState::LobbyState(StateStack& stack, Context context, const bool is_host)
	: State(stack, context)
	  , m_player_input_name(context.m_player_data_manager->GetData().m_player_name)
	  , m_connected(false)
	  , m_is_host(is_host)
	  , m_unpaired_y_pos(200)
	  , m_player(-1)
	  , m_time_since_last_packet(sf::seconds(0.f))
	  , m_client_timeout(sf::seconds(2.f))
	  , m_lobby_time(sf::seconds(0))
	  , m_send_time(sf::seconds(0.5f))
{
	CreateUI(context);


	sf::IpAddress ip;
	if (m_is_host)
	{
		context.m_game_server = new GameServer(sf::Vector2f(context.m_window->getSize()));
		ip = "127.0.0.1";
	}
	else
	{
		ip = context.m_player_data_manager->GetData().m_ip_address;
	}


	if (sf::TcpSocket::Done == context.m_socket->connect(ip, SERVER_PORT, sf::seconds(10.f)))
	{
		m_connected = true;
	}
	else
	{
		m_failed_connection_clock.restart();
	}

	context.m_socket->setBlocking(false);

	for (sf::Int8 i = 0; i < 8; ++i)
	{
		m_team_selections.try_emplace(i, std::vector<sf::Int8>());
	}
}

bool LobbyState::TeamHasPlace(const sf::Int8 id)
{
	if (m_team_selections[id].size() < 2)
	{
		return true;
	}

	return false;
}

sf::Vector2f LobbyState::GetTeamPos(const int i)
{
	const int y = 150 + 150 * ((i - 1 - (i - 1) % 2) / 2);
	const int x = i % 2 == 0 ? 900 : 500;

	return {static_cast<float>(x), static_cast<float>(y)};
}

void LobbyState::MovePlayer(const sf::Int8 id, const sf::Int8 team_id)
{
	if (m_player_team_selection[id] != 0)
	{
		m_team_selections[m_player_team_selection[id]].erase(
			std::remove(m_team_selections[m_player_team_selection[id]].begin(),
			            m_team_selections[m_player_team_selection[id]].end(), id),
			m_team_selections[m_player_team_selection[id]].end());
	}

	m_team_selections[team_id].emplace_back(id);

	const sf::Vector2f pos = GetTeamPos(team_id);
	float y = pos.y;

	if (m_team_selections[team_id].front() != id)
	{
		y += 2 * 60;
	}
	else
	{
		y += 60;
	}

	m_players[id]->setPosition(pos.x, y);

	m_player_team_selection[id] = team_id;
}

void LobbyState::HandleTeamChoice(const sf::Int8 i)
{
	if (TeamHasPlace(i))
	{
		sf::Packet packet;
		packet << static_cast<sf::Int8>(client::PacketType::kChoseTeam);
		packet << m_player;
		packet << i;

		m_context.m_socket->send(packet);
	}
}

void LobbyState::Draw()
{
	sf::RenderWindow& window = *GetContext().m_window;


	if (m_connected)
	{
		window.clear(sf::Color(0, 37, 97));
		window.draw(m_gui_container);
	}
	else
	{
		window.clear(sf::Color(0, 37, 97));
		window.draw(m_gui_fail_container);
	}
}

void LobbyState::NotifyServerOfExistence() const
{
	sf::Packet packet;
	m_context.m_socket->send(packet);
}

bool LobbyState::Update(const sf::Time dt)
{
	if (m_connected)
	{
		if (m_lobby_time > m_send_time)
		{
			m_lobby_time = sf::seconds(0.f);
			NotifyServerOfExistence();
		}

		//Handle messages from the server that may have arrived
		sf::Packet packet;
		if (m_context.m_socket->receive(packet) == sf::Socket::Done)
		{
			m_time_since_last_packet = sf::seconds(0.f);
			sf::Int8 packet_type;
			packet >> packet_type;
			HandlePacket(packet_type, packet);
		}
		else
		{
			if (m_time_since_last_packet > m_client_timeout)
			{
				m_connected = false;
				m_failed_connection_text->SetText("Lost connection to the server");
				Utility::CentreOrigin(m_failed_connection_text->GetText());

				m_failed_connection_clock.restart();
			}
		}
	}
	//Failed to connect and waited for more than 5 seconds: Back to menu
	else if (m_failed_connection_clock.getElapsedTime() >= sf::seconds(5.f))
	{
		RequestStackClear();
		RequestStackPush(StateID::kMenu);
	}

	m_time_since_last_packet += dt;
	m_lobby_time += dt;
	return true;
}

bool LobbyState::HandleEvent(const sf::Event& event)
{
	if (m_change_name_button->IsActive())
	{
		//Name Input
		if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Return)
		{
			m_change_name_button->Deactivate();
			GetContext().m_player_data_manager->GetData().m_player_name = m_player_input_name;
			GetContext().m_player_data_manager->Save();

			m_players[m_player]->SetText(m_player_input_name);

			SendPlayerName(m_player, m_player_input_name);

		}
		else if (event.type == sf::Event::TextEntered)
		{
			if (event.text.unicode == '\b')
			{
				if (!m_player_input_name.empty())
					m_player_input_name.erase(m_player_input_name.size() - 1, 1);
			}
			else if (event.text.unicode != '\n' && event.text.unicode != '\r')
			{
				m_player_input_name += event.text.unicode;
				m_player_input_name = m_player_input_name.substr(0, 20);
			}

			m_current_name_label->SetText(m_player_input_name);
		}
	}
	else
	{
		m_gui_container.HandleEvent(event);
	}

	return false;
}

void LobbyState::HandleTeamSelection(sf::Packet& packet)
{
	sf::Int8 identifier;
	sf::Int8 team_identifier;
	packet >> identifier >> team_identifier;

	Utility::Debug(std::to_string(identifier) + " : " + std::to_string(team_identifier));

	MovePlayer(identifier, team_identifier);
}


void LobbyState::HandleGameStart()
{
	RequestStackPop();
	RequestStackPush(StateID::kNetworkGame);
}

void LobbyState::HandlePacket(sf::Int8 packet_type, sf::Packet& packet)
{
	switch (static_cast<server::PacketType>(packet_type))
	{
	case server::PacketType::kSpawnSelf:
		HandleSpawnSelf(packet);
		break;
	case server::PacketType::kPlayerConnect:
		HandlePlayerConnect(packet);
		break;
	case server::PacketType::kPlayerDisconnect:
		HandlePlayerDisconnect(packet);
		break;
	case server::PacketType::kInitialState:
		HandleInitialState(packet);
		break;
	case server::PacketType::kUpdatePlayer:
		HandleUpdatePlayer(packet);
		break;
	case server::PacketType::kTeamSelection:
		HandleTeamSelection(packet);
		break;
	case server::PacketType::kStartGame:
		HandleGameStart();
		break;
	default:
		break;
	}
}

void LobbyState::HandlePlayerConnect(sf::Packet& packet)
{
	sf::Int8 identifier;
	packet >> identifier;
	AddPlayer(identifier, "Default");
}

void LobbyState::HandlePlayerDisconnect(sf::Packet& packet)
{
	sf::Int8 id;
	packet >> id;

	auto& team_selection = m_team_selections[m_player_team_selection[id]];
	const auto remove = std::remove(team_selection.begin(), team_selection.end(), id);
	team_selection.erase(remove, team_selection.end());

	const auto remove_team_selection = m_team_selections.find(id);
	m_team_selections.erase(remove_team_selection, m_team_selections.end());

	const auto remove_player = m_players.find(id);
	m_players.erase(remove_player, m_players.end());
}

void LobbyState::HandleUpdatePlayer(sf::Packet& packet)
{
	sf::Int8 identifier;
	std::string name;

	packet >> identifier >> name;
	
	m_players[identifier]->SetText(name);
}

void LobbyState::HandleInitialState(sf::Packet& packet)
{
	sf::Int8 player_count;
	packet >> player_count;
	for (sf::Int8 i = 0; i < player_count; ++i)
	{
		sf::Int8 identifier;
		sf::Int8 team_identifier;
		std::string name;

		packet >> identifier >> team_identifier >> name;

		AddPlayer(identifier, name);

		if (team_identifier != 0)
		{
			if (identifier != m_player)
			{
				MovePlayer(identifier, team_identifier);
			}
		}
	}
}

void LobbyState::SendPlayerName(const sf::Int8 identifier, const std::string& name) const
{
	sf::Packet packet;
	packet << static_cast<sf::Int8>(client::PacketType::kPlayerUpdate);
	packet << identifier;
	packet << name;

	m_context.m_socket->send(packet);
}

void LobbyState::AddPlayer(const sf::Int8 identifier, const std::string& label_text)
{
	GUI::Label::Ptr name;
	Utility::CreateLabel(m_context, name, 80, m_unpaired_y_pos + 30 * identifier,
	                     label_text, 20);
	m_gui_container.Pack(name);
	m_players.try_emplace(identifier, name);
	m_player_team_selection.try_emplace(identifier, 0);
}

void LobbyState::HandleSpawnSelf(sf::Packet& packet)
{
	sf::Int8 identifier;
	packet >> identifier;

	m_player = identifier;

	Utility::Debug("Player connected.");
	AddPlayer(identifier, GetContext().m_player_data_manager->GetData().m_player_name);
	SendPlayerName(identifier, GetContext().m_player_data_manager->GetData().m_player_name);
}
