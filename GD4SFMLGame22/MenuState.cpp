#include "MenuState.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

#include "ResourceHolder.hpp"
#include "Utility.hpp"
#include "Button.hpp"

MenuState::MenuState(StateStack& stack, Context& context)
	: State(stack, context)
{
	sf::Texture& texture = context.m_textures->Get(Textures::kTitleScreen);

	m_background_sprite.setTexture(texture);

	//todo coop play button removed, because coop mode is currently broken
	/*std::shared_ptr<GUI::Button> play_button;
	CreateButton(context, play_button, 100, 250, "Play", [this]
	{
		RequestStackPop();
		RequestStackPush(StateID::kGame);
	});*/

	std::shared_ptr<GUI::Button> host_button;
	Utility::CreateButton(context, host_button, 100, 300, "Host", [this]
	{
		RequestStackPop();
		RequestStackPush(StateID::kLobbyHost);
	});

	std::shared_ptr<GUI::Button> join_button;
	Utility::CreateButton(context, join_button, 100, 350, "Join", [this]
	{
		RequestStackPop();
		RequestStackPush(StateID::kJoinSettings);
	});

	std::shared_ptr<GUI::Button> settings_button;
	Utility::CreateButton(context, settings_button, 100, 400, "Settings", [this]
	{
		RequestStackPush(StateID::kSettings);
	});

	std::shared_ptr<GUI::Button> exit_button;
	Utility::CreateButton(context, exit_button, 100, 450, "Exit", [this]
	{
		RequestStackPop();
	});

	/*m_gui_container.Pack(play_button);*/
	m_gui_container.Pack(host_button);
	m_gui_container.Pack(join_button);
	m_gui_container.Pack(settings_button);
	m_gui_container.Pack(exit_button);

	// Play menu theme
	context.m_music->Play(MusicThemes::kMenuTheme);
}

void MenuState::Draw()
{
	sf::RenderWindow& window = *GetContext().m_window;
	window.setView(window.getDefaultView());
	window.draw(m_background_sprite);
	window.draw(m_gui_container);
}

bool MenuState::Update(sf::Time dt)
{
	return true;
}

bool MenuState::HandleEvent(const sf::Event& event)
{
	m_gui_container.HandleEvent(event);
	return false;
}
