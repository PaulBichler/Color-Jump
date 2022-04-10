#include "MultiplayerGameOverState.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "Utility.hpp"

//Written by Paul Bichler (D00242563)
MultiplayerGameOverState::MultiplayerGameOverState(StateStack& stack, Context& context, const bool has_won)
	: State(stack, context)
{
	const sf::Font& font = context.m_fonts->Get(Fonts::kMain);
	const sf::Vector2f viewSize = context.m_window->getView().getSize();

	//Create the Win/Lose Title Text
	if(has_won)
	{
		m_title_text.setString("You Win!");
		m_title_text.setFillColor(sf::Color::Green);
	}
	else
	{
		m_title_text.setString("You Lose!");
		m_title_text.setFillColor(sf::Color::Red);
	}

	m_title_text.setFont(font);
	m_title_text.setCharacterSize(70);
	Utility::CentreOrigin(m_title_text);
	m_title_text.setPosition(0.5f * viewSize.x, 0.2f * viewSize.y);

	//Create the leaderboard labels
	context.m_multiplayer_manager->SetLeaderboardChangeCallback([this]{ UpdateLeaderboard(); });
	const int number_of_teams = GetContext().m_multiplayer_manager->GetNumberOfTeams();
	for(int i = 0; i < number_of_teams; i++)
	{
		const auto label = std::make_shared<GUI::Label>("", *GetContext().m_fonts, 25.f);
		label->setPosition(250.f, 300.f + i * 35.f);
		m_leaderboard.emplace_back(label.get());
		m_gui_container.Pack(label);
	}
	UpdateLeaderboard();

	//Create the Back to Main Menu button
	const auto main_menu_button = std::make_shared<GUI::Button>(context);
	main_menu_button->setPosition(0.5f * viewSize.x - 100, 0.8f * viewSize.y);
	main_menu_button->SetText("Back to Main Menu");
	main_menu_button->SetCallback([this]()
	{
		GetContext().m_multiplayer_manager->Disconnect();
		RequestStackClear();
		RequestStackPush(StateID::kMenu);
	});
	m_gui_container.Pack(main_menu_button);
}

//Written by Paul Bichler (D00242563)
void MultiplayerGameOverState::Draw()
{
	sf::RenderWindow& window = *GetContext().m_window;
	window.setView(window.getDefaultView());

	sf::RectangleShape backgroundShape;
	backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
	backgroundShape.setSize(window.getView().getSize());

	window.draw(backgroundShape);
	window.draw(m_title_text);
	window.draw(m_gui_container);
}

//Written by Paul Bichler (D00242563)
bool MultiplayerGameOverState::Update(sf::Time dt)
{
	//This state is pushed onto the MultiplayerGameState, which needs to update as well,
	//otherwise the client will be timed out
	return true;
}

//Written by Paul Bichler (D00242563)
bool MultiplayerGameOverState::HandleEvent(const sf::Event& event)
{
	m_gui_container.HandleEvent(event);
	return false;
}

//Written by Paul Bichler (D00242563)
void MultiplayerGameOverState::UpdateLeaderboard() const
{
	const auto leaderboard = GetContext().m_multiplayer_manager->GetLeaderboard();

	//Go through all of the players in the leaderboard (stored in the context) and update the labels
	for(int i = 0; i < leaderboard.size(); i++)
	{
		auto player_names = GetContext().m_multiplayer_manager->GetPlayerNamesOfTeam(leaderboard[i].first);
		std::string team_id = std::to_string(leaderboard[i].first);
		std::string completion_time = std::to_string(leaderboard[i].second.asSeconds());
		std::string text = std::to_string(i + 1) + ".\tTeam " + team_id + "\t(" + player_names[0] + " & " + player_names[1] + ")\tTime: " + completion_time + " s";
		m_leaderboard[i]->SetText(text);
	}
}
