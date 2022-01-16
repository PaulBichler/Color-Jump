#include "LevelPauseState.hpp"
#include "ResourceHolder.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

#include "Button.hpp"
#include "Utility.hpp"

LevelPauseState::LevelPauseState(StateStack& stack, Context context)
	: State(stack, context)
{
	const sf::Font& font = context.fonts->Get(Fonts::Main);
	const sf::Vector2f viewSize = context.window->getView().getSize();

	//Game Paused Title Text
	m_paused_text.setFont(font);
	m_paused_text.setString("Game Paused!");
	m_paused_text.setCharacterSize(70);
	Utility::CentreOrigin(m_paused_text);
	m_paused_text.setPosition(0.5f * viewSize.x, 0.4f * viewSize.y);

	//Create Resume Button
	const auto resume_button = std::make_shared<GUI::Button>(context);
	resume_button->setPosition(0.5f * viewSize.x - 100, 0.4f * viewSize.y + 100);
	resume_button->SetText("Resume");
	resume_button->SetCallback([this]()
	{
		RequestStackPop(); //Pop Level Pause State
	});

	//Create Restart Button
	const auto restart_button = std::make_shared<GUI::Button>(context);
	restart_button->setPosition(0.5f * viewSize.x - 100, 0.4f * viewSize.y + 150);
	restart_button->SetText("Restart");
	restart_button->SetCallback([this]()
	{
		RequestStackPop(); //Pop Level Pause State
		RequestStackPop(); //Pop Game State
		RequestStackPush(StateID::kGame); //Push Game State again to restart the level
	});

	//Create Back to Main Menu Button
	const auto main_menu_button = std::make_shared<GUI::Button>(context);
	main_menu_button->setPosition(0.5f * viewSize.x - 100, 0.4f * viewSize.y + 200);
	main_menu_button->SetText("Back to Main Menu");
	main_menu_button->SetCallback([this]()
	{
		RequestStackClear();
		RequestStackPush(StateID::kMenu);
	});

	m_gui_container.Pack(resume_button);
	m_gui_container.Pack(restart_button);
	m_gui_container.Pack(main_menu_button);
}

void LevelPauseState::Draw()
{
	sf::RenderWindow& window = *GetContext().window;
	window.setView(window.getDefaultView());

	sf::RectangleShape backgroundShape;
	backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
	backgroundShape.setSize(window.getView().getSize());

	window.draw(backgroundShape);
	window.draw(m_paused_text);
	window.draw(m_gui_container);
}

bool LevelPauseState::Update(sf::Time)
{
	return false;
}

bool LevelPauseState::HandleEvent(const sf::Event& event)
{
	m_gui_container.HandleEvent(event);

	if (event.type != sf::Event::KeyPressed)
		return false;

	if (event.key.code == sf::Keyboard::Escape)
	{
		// Escape pressed, remove itself to return to the game
		RequestStackPop();
	}

	return false;
}