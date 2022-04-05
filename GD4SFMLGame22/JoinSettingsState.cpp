#include "JoinSettingsState.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

#include "Button.hpp"
#include "Label.hpp"

JoinSettingsState::JoinSettingsState(StateStack& stack, Context& context)
	: State(stack, context),
	m_player_input_ip(context.m_player_data_manager->GetData().m_ip_address),
	m_player_input_name(context.m_player_data_manager->GetData().m_player_name)
{
	const sf::Texture& texture = context.m_textures->Get(Textures::kTitleScreen);
	m_background_sprite.setTexture(texture);

	//Name Input
	m_change_name_button = std::make_shared<GUI::Button>(context);
	m_change_name_button->SetText("Change Name");
	m_change_name_button->SetToggle(true);
	m_change_name_button->setPosition(80.f, 250.f);

	m_current_name_label = std::make_shared<GUI::Label>(m_player_input_name, *context.m_fonts, 20);
	m_current_name_label->setPosition(310.f, 265.f);

	m_gui_container.Pack(m_change_name_button);
	m_gui_container.Pack(m_current_name_label);

	//Ip Input
	m_change_ip_button = std::make_shared<GUI::Button>(context);
	m_change_ip_button->SetText("Change IP Address");
	m_change_ip_button->SetToggle(true);
	m_change_ip_button->setPosition(80.f, 300.f);

	m_current_ip_label = std::make_shared<GUI::Label>(m_player_input_ip, *context.m_fonts, 20);
	m_current_ip_label->setPosition(310.f, 315.f);

	m_gui_container.Pack(m_change_ip_button);
	m_gui_container.Pack(m_current_ip_label);

	//Connect Button
	const auto connect_button = std::make_shared<GUI::Button>(context);
	connect_button->setPosition(80.f, 400.f);
	connect_button->SetText("Connect");
	connect_button->SetCallback([this]
	{
		RequestStackPop(); //Pop Menu State
		RequestStackPush(StateID::kLobbyClient);
	});

	m_gui_container.Pack(connect_button);

	//Back Button
	const auto back_button = std::make_shared<GUI::Button>(context);
	back_button->setPosition(80.f, 450.f);
	back_button->SetText("Back");
	back_button->SetCallback([this]
	{
		RequestStackPop();
	});

	m_gui_container.Pack(back_button);
}

void JoinSettingsState::Draw()
{
	sf::RenderWindow& window = *GetContext().m_window;
	window.draw(m_background_sprite);
	window.draw(m_gui_container);
}

bool JoinSettingsState::Update(sf::Time dt)
{
	return true;
}

bool JoinSettingsState::HandleEvent(const sf::Event& event)
{
	if (m_change_ip_button->IsActive())
	{
		//Ip Input
		if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Return)
		{
			m_change_ip_button->Deactivate();
			GetContext().m_player_data_manager->GetData().m_ip_address = m_player_input_ip;
			GetContext().m_player_data_manager->Save();
		}
		else if (event.type == sf::Event::TextEntered)
		{
			if (event.text.unicode == '\b')
			{
				if (!m_player_input_ip.empty())
					m_player_input_ip.erase(m_player_input_ip.size() - 1, 1);
			}
			else if (event.text.unicode != '\n' && event.text.unicode != '\r')
			{
				m_player_input_ip += event.text.unicode;
				m_player_input_ip = m_player_input_ip.substr(0, 25);
			}

			m_current_ip_label->SetText(m_player_input_ip);
		}
	}
	else if (m_change_name_button->IsActive())
	{
		//Name Input
		if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Return)
		{
			m_change_name_button->Deactivate();
			GetContext().m_player_data_manager->GetData().m_player_name = m_player_input_name;
			GetContext().m_player_data_manager->Save();
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
