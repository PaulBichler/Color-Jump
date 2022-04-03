#include "LevelWinState.hpp"
#include "ResourceHolder.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

#include "Button.hpp"
#include "Utility.hpp"

LevelWinState::LevelWinState(StateStack& stack, Context context)
	: State(stack, context)
{
	const sf::Font& font = context.m_fonts->Get(Fonts::Main);
	const sf::Vector2f viewSize = context.m_window->getView().getSize();

	//Create the Win Title Text
	m_lost_text.setFont(font);
	m_lost_text.setString("You Win!");
	m_lost_text.setFillColor(sf::Color::Green);
	m_lost_text.setCharacterSize(70);
	Utility::CentreOrigin(m_lost_text);
	m_lost_text.setPosition(0.5f * viewSize.x, 0.4f * viewSize.y);

	//Create the Next Level button
	const auto next_level_button = std::make_shared<GUI::Button>(context);
	next_level_button->setPosition(0.5f * viewSize.x - 100, 0.4f * viewSize.y + 100);
	next_level_button->SetText("Next Level");
	next_level_button->SetCallback([this]()
	{
		//Set the next level in the level manager
		GetContext().m_level_manager->NextLevel();
	
		RequestStackPop(); //Pop Level Lose State
		RequestStackPop(); //Pop Game State
		RequestStackPush(StateID::kGame); //Push Game State again to restart the level
	});

	//The next level button should not be drawn when no last level exists
	next_level_button->SetDrawPredicate([this]
	{
		return GetContext().m_level_manager->DoesNextLevelExist();
	});

	// //Create the Restart button
	// const auto restart_button = std::make_shared<GUI::Button>(context);
	// restart_button->setPosition(0.5f * viewSize.x - 100, 0.4f * viewSize.y + 150);
	// restart_button->SetText("Restart Level");
	// restart_button->SetCallback([this]()
	// {
	// 	RequestStackPop(); //Pop Level Win State
	// 	RequestStackPop(); //Pop Game State
	// 	RequestStackPush(StateID::kGame); //Push Game State again to restart the level
	// });

	//Create the Back to Main Menu button
	const auto main_menu_button = std::make_shared<GUI::Button>(context);
	main_menu_button->setPosition(0.5f * viewSize.x - 100, 0.4f * viewSize.y + 200);
	main_menu_button->SetText("Back to Main Menu");
	main_menu_button->SetCallback([this]()
	{
		RequestStackClear();
		RequestStackPush(StateID::kMenu);
	});

	m_gui_container.Pack(next_level_button);
	// m_gui_container.Pack(restart_button);
	m_gui_container.Pack(main_menu_button);
}

void LevelWinState::Draw()
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

bool LevelWinState::Update(sf::Time)
{
	return false;
}

bool LevelWinState::HandleEvent(const sf::Event& event)
{
	m_gui_container.HandleEvent(event);
	return false;
}
