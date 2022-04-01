#include "SettingsState.hpp"
#include "Utility.hpp"
#include "ResourceHolder.hpp"
#include "StateStack.hpp"

#include <SFML/Graphics/RenderWindow.hpp>


SettingsState::SettingsState(StateStack& stack, Context context)
	: State(stack, context),
	m_player_input(context.m_player_data_manager->GetData().m_player_name)
{
	m_background_sprite.setTexture(context.m_textures->Get(Textures::kTitleScreen));

	m_change_name_button = std::make_shared<GUI::Button>(context);
	m_change_name_button->SetText("Change Name");
	m_change_name_button->SetToggle(true);
	m_change_name_button->setPosition(80.f, 250.f);


	m_current_name_label = std::make_shared<GUI::Label>(m_player_input, *context.m_fonts);
	m_current_name_label->setPosition(300.f, 265.f);

	m_gui_container.Pack(m_change_name_button);
	m_gui_container.Pack(m_current_name_label);

	for (std::size_t x = 0; x < 2; ++x)
	{
		AddButtonLabel(static_cast<int>(PlayerAction::kMoveLeft), x, 0, "Move Left", context);
		AddButtonLabel(static_cast<int>(PlayerAction::kMoveRight), x, 1, "Move Right", context);
		AddButtonLabel(static_cast<int>(PlayerAction::kMoveUp), x, 2, "Move Up", context);
	}

	UpdateLabels();

	const auto back_button = std::make_shared<GUI::Button>(context);
	back_button->setPosition(80.f, 620.f);
	back_button->SetText("Back");
	back_button->SetCallback([this] { RequestStackPop(); });

	m_gui_container.Pack(back_button);
}

void SettingsState::Draw()
{
	sf::RenderWindow& window = *GetContext().m_window;

	window.draw(m_background_sprite);
	window.draw(m_gui_container);
}

bool SettingsState::Update(sf::Time)
{
	return true;
}

bool SettingsState::HandleEvent(const sf::Event& event)
{
	bool is_key_binding = false;

	// Iterate through all key binding buttons to see if they are being pressed, waiting for the user to enter a key
	for (std::size_t i = 0; i < 2 * static_cast<int>(PlayerAction::kActionCount); ++i)
	{
		if (m_binding_buttons[i]->IsActive())
		{
			is_key_binding = true;
			if (event.type == sf::Event::KeyReleased)
			{
				// Player 1
				if (i < static_cast<int>(PlayerAction::kActionCount))
					GetContext().m_keys1->AssignKey(static_cast<PlayerAction>(i), event.key.code);

					// Player 2
				else
					GetContext().m_keys2->AssignKey(
						static_cast<PlayerAction>(i - static_cast<int>(PlayerAction::kActionCount)),
						event.key.code);

				m_binding_buttons[i]->Deactivate();
			}
			break;
		}
	}


	if (m_change_name_button->IsActive())
	{
		if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Return)
		{
			m_change_name_button->Deactivate();
			GetContext().m_player_data_manager->GetData().m_player_name = m_player_input;
		}
		else if (event.type == sf::Event::TextEntered)
		{
			if (event.text.unicode == '\b') 
			{
				if (!m_player_input.empty())
					m_player_input.erase(m_player_input.size() - 1, 1);
			}
			else if(event.text.unicode != '\n' && event.text.unicode != '\r')
			{
				m_player_input += event.text.unicode;
				m_player_input = m_player_input.substr(0, 20);
			}

			m_current_name_label->SetText(m_player_input);
		}
	}
	else if (is_key_binding) 
	{
		// If pressed button changed key bindings, update labels;
		UpdateLabels();
	}
	else
	{
		m_gui_container.HandleEvent(event);
	}

	return false;
}

void SettingsState::UpdateLabels() const
{
	for (std::size_t i = 0; i < static_cast<int>(PlayerAction::kActionCount); ++i)
	{
		const auto action = static_cast<PlayerAction>(i);

		// Get keys of both players
		const sf::Keyboard::Key key1 = GetContext().m_keys1->GetAssignedKey(action);
		const sf::Keyboard::Key key2 = GetContext().m_keys2->GetAssignedKey(action);

		// Assign both key strings to labels
		m_binding_labels[i]->SetText(Utility::ToString(key1));
		m_binding_labels[i + static_cast<int>(PlayerAction::kActionCount)]->SetText(
			Utility::ToString(key2));
	}
}

void SettingsState::AddButtonLabel(std::size_t index, const size_t x, const size_t y, const std::string& text, Context context)
{
	index += static_cast<int>(PlayerAction::kActionCount) * x;

	m_binding_buttons[index] = std::make_shared<GUI::Button>(context);
	const auto x_pos = static_cast<float>(x);
	const auto y_pos = static_cast<float>(y);

	m_binding_buttons[index]->setPosition(400.f * x_pos + 80.f, 50.f * y_pos + 350.f);
	m_binding_buttons[index]->SetText(text);
	m_binding_buttons[index]->SetToggle(true);

	m_binding_labels[index] = std::make_shared<GUI::Label>("", *context.m_fonts);
	m_binding_labels[index]->setPosition(400.f * x_pos + 300.f, 50.f * y_pos + 365.f);

	m_gui_container.Pack(m_binding_buttons[index]);
	m_gui_container.Pack(m_binding_labels[index]);
}
