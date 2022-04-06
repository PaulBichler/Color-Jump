#include "TutorialState.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "Utility.hpp"

TutorialState::TutorialState(StateStack& stack, Context& context) : State(stack, context)
{
	const int middle_x = context.m_window->getSize().x / 2;

	std::shared_ptr<GUI::Label> title_text;
	Utility::CreateLabel(context, title_text, 550, 50, "Welcome to Color Jump!", 60);
	m_gui_container.Pack(title_text);

	std::shared_ptr<GUI::Label> tutorial_text;
	Utility::CreateLabel(context, tutorial_text, 500, 200, 
		"In Color Jump, you play in a team of 2, one Red and one Blue character. Characters can\nonly collide with platforms of their color or white platforms.\n\n\n\n\n\n\n\nSome platforms change their color to the color of the character that collides with them.\nThe goal of Color Jump is for your team to complete the obstacle course and reach the end\nplatform before any other team. The level features some tricky puzzles that require you and\nyour team mate to carefully think about your moves. Made a wrong move? Jumping out of\nthe play field resets the puzzle for both players.\n\n\n\n\n\n\n\n\nDon't worry, there's a checkpoint after each puzzle that is activated when both players stand\non it (it turns green if it's activated).\n\nHappy Jumping!"
		, 20);
	m_gui_container.Pack(tutorial_text);

	std::shared_ptr<GUI::Button> back_button;
	Utility::CreateButton(context, back_button, middle_x - 100, 850, "OK", [this]
		{
			RequestStackPop();
		});
	m_gui_container.Pack(back_button);

	m_tutorial1.setTexture(context.m_textures->Get(Textures::kTutorialColorCollision));
	m_tutorial1.setScale(.37f, .37f);
	m_tutorial1.setPosition(100, 50);

	m_tutorial2.setTexture(context.m_textures->Get(Textures::kTutorialImpactPlatform));
	m_tutorial2.setScale(.37f, .37f);
	m_tutorial2.setPosition(100, 350);

	m_tutorial3.setTexture(context.m_textures->Get(Textures::kTutorialCheckpoint));
	m_tutorial3.setScale(.37f, .37f);
	m_tutorial3.setPosition(100, 650);
}

void TutorialState::Draw()
{
	sf::RenderWindow& window = *GetContext().m_window;
	window.clear(sf::Color(0, 37, 80));
	window.draw(m_gui_container);
	window.draw(m_tutorial1);
	window.draw(m_tutorial2);
	window.draw(m_tutorial3);
}

bool TutorialState::Update(sf::Time dt)
{
	return true;
}

bool TutorialState::HandleEvent(const sf::Event& event)
{
	m_gui_container.HandleEvent(event);
	return false;
}
