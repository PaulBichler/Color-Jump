#pragma once
#include <SFML/Graphics/Sprite.hpp>

#include "Button.hpp"
#include "Container.hpp"
#include "Label.hpp"
#include "State.hpp"

class LobbyState : public State
{
public:
	void CreateUI(Context context);
	LobbyState(StateStack& stack, Context context, bool is_host);
	bool TeamHasPlace(sf::Int8 i);
	static sf::Vector2f GetTeamPos(int i);
	void MovePlayer(sf::Int8 id, sf::Int8 team_id);
	void HandleTeamChoice(sf::Int8 i);
	void Draw() override;
	bool Update(sf::Time dt) override;
	bool HandleEvent(const sf::Event& event) override;


private:
	void HandleTeamSelection(sf::Packet& packet);
	void HandleGameStart();
	void HandlePacket(sf::Int8 packet_type, sf::Packet& packet);
	void HandlePlayerConnect(sf::Packet& packet);
	void HandlePlayerDisconnect(sf::Packet& packet);
	void HandleUpdatePlayer(sf::Packet& packet);
	void HandleInitialState(sf::Packet& packet);
	void SendPlayerName(sf::Int8 identifier, sf::Int8 team_id, const std::string& name);
	void AddPlayer(sf::Int8 identifier, const std::string& label_text);
	void HandleSpawnSelf(sf::Packet& packet);

	sf::Sprite m_background_sprite;
	GUI::Container m_gui_container;
	GUI::Container m_gui_fail_container;

	GUI::Button::Ptr m_change_name_button;
	GUI::Label::Ptr m_current_name_label;
	std::string m_player_input_name;

	GUI::Label::Ptr m_failed_connection_text;
	sf::Clock m_failed_connection_clock;

	bool m_connected;
	bool m_is_host;
	int m_unpaired_y_pos = 200;

	std::map<sf::Int8, GUI::Label::Ptr> m_players;
	std::map<sf::Int8, std::vector<sf::Int8>> m_team_selections;
	std::map<sf::Int8, sf::Int8> m_player_team_selection;
	sf::Int8 m_player{};
};
