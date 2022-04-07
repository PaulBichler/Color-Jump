//Written by Paul Bichler (D00242563)

#include "LevelLoseState.hpp"
#include "ResourceHolder.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

#include "Button.hpp"
#include "Utility.hpp"

//Written by Paul Bichler (D00242563)
//The Level Lose State is pushed when a player dies
LevelLoseState::LevelLoseState(StateStack& stack, Context& context)
	: State(stack, context)
{
	const sf::Font& font = context.m_fonts->Get(Fonts::kMain);
	const sf::Vector2f viewSize = context.m_window->getView().getSize();

	//Create the Lose Level title text
	m_lost_text.setFont(font);
	m_lost_text.setString("You Lost!");
	m_lost_text.setFillColor(sf::Color::Red);
	m_lost_text.setCharacterSize(70);
	Utility::CentreOrigin(m_lost_text);
	m_lost_text.setPosition(0.5f * viewSize.x, 0.4f * viewSize.y);

	// //Create the Restart button
	// const auto restart_button = std::make_shared<GUI::Button>(context);
	// restart_button->setPosition(0.5f * viewSize.x - 100, 0.4f * viewSize.y + 100);
	// restart_button->SetText("Restart");
	// restart_button->SetCallback([this]()
	// {
	// 	RequestStackPop(); //Pop Level Lose State
	// 	RequestStackPop(); //Pop Game State
	// 	RequestStackPush(StateID::kGame); //Push Game State again to restart the level
	// });

	//Create the Back to Main Menu button
	const auto main_menu_button = std::make_shared<GUI::Button>(context);
	main_menu_button->setPosition(0.5f * viewSize.x - 100, 0.4f * viewSize.y + 150);
	main_menu_button->SetText("Back to Main Menu");
	main_menu_button->SetCallback([this]()
	{
		GetContext().DisableServer();
		RequestStackClear();
		RequestStackPush(StateID::kMenu);
	});

	// m_gui_container.Pack(restart_button);
	m_gui_container.Pack(main_menu_button);
}

//Written by Paul Bichler (D00242563)
void LevelLoseState::Draw()
{
	sf::RenderWindow& window = *GetContext().m_window;
	window.setView(window.getDefaultView());

	sf::RectangleShape backgroundShape;
	backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
	backgroundShape.setSize(window.getView().getSize());

	window.draw(backgroundShape);
	window.draw(m_lost_text);
	window.draw(m_gui_container);
}

//Written by Paul Bichler (D00242563)
bool LevelLoseState::Update(sf::Time)
{
	return false;
}

//Written by Paul Bichler (D00242563)
bool LevelLoseState::HandleEvent(const sf::Event& event)
{
	m_gui_container.HandleEvent(event);
	return false;
}
