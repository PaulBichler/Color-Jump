//Written by Paul Bichler (D00242563)

#include "LevelWinState.hpp"
#include "ResourceHolder.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

#include "Button.hpp"
#include "Utility.hpp"

//Written by Paul Bichler (D00242563)
//The Level Win State is pushed after a level is completed successfully
LevelWinState::LevelWinState(StateStack& stack, const Context context)
	: State(stack, context)
{
	const sf::Font& font = context.fonts->Get(Fonts::Main);
	const sf::Vector2f viewSize = context.window->getView().getSize();

	m_lost_text.setFont(font);
	m_lost_text.setString("You Win!");
	m_lost_text.setFillColor(sf::Color::Green);
	m_lost_text.setCharacterSize(70);
	Utility::CentreOrigin(m_lost_text);
	m_lost_text.setPosition(0.5f * viewSize.x, 0.4f * viewSize.y);

	if(context.level_manager->DoesNextLevelExist()) 
	{
		const auto next_level_button = std::make_shared<GUI::Button>(context);
		next_level_button->setPosition(0.5f * viewSize.x - 100, 0.4f * viewSize.y + 50);
		next_level_button->SetText("Next Level");
		next_level_button->SetCallback([this]()
		{
			//Set the next level in the level manager
			GetContext().level_manager->NextLevel();

			RequestStackPop(); //Pop Level Lose State
			RequestStackPop(); //Pop Game State
			RequestStackPush(StateID::kGame); //Push Game State again to restart the level
		});

		m_gui_container.Pack(next_level_button);
	}

	const auto restart_button = std::make_shared<GUI::Button>(context);
	restart_button->setPosition(0.5f * viewSize.x - 100, 0.4f * viewSize.y + 100);
	restart_button->SetText("Restart Level");
	restart_button->SetCallback([this]()
	{
		RequestStackPop(); //Pop Level Win State
		RequestStackPop(); //Pop Game State
		RequestStackPush(StateID::kGame); //Push Game State again to restart the level
	});

	const auto main_menu_button = std::make_shared<GUI::Button>(context);
	main_menu_button->setPosition(0.5f * viewSize.x - 100, 0.4f * viewSize.y + 150);
	main_menu_button->SetText("Back to Main Menu");
	main_menu_button->SetCallback([this]()
	{
		RequestStackClear();
		RequestStackPush(StateID::kMenu);
	});

	m_gui_container.Pack(restart_button);
	m_gui_container.Pack(main_menu_button);
}

//Written by Paul Bichler (D00242563)
void LevelWinState::Draw()
{
	sf::RenderWindow& window = *GetContext().window;
	window.setView(window.getDefaultView());

	sf::RectangleShape backgroundShape;
	backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
	backgroundShape.setSize(window.getView().getSize());

	window.draw(backgroundShape);
	window.draw(m_lost_text);
	window.draw(m_gui_container);
}

//Written by Paul Bichler (D00242563)
bool LevelWinState::Update(sf::Time)
{
	return false;
}

//Written by Paul Bichler (D00242563)
bool LevelWinState::HandleEvent(const sf::Event& event)
{
	m_gui_container.HandleEvent(event);
	return false;
}
