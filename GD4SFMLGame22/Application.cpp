#include "Application.hpp"

#include "State.hpp"
#include "StateID.hpp"
#include "TitleState.hpp"
#include "GameState.hpp"
#include "LevelLoseState.hpp"
#include "LevelWinState.hpp"
#include "MenuState.hpp"
#include "LevelPauseState.hpp"
#include "MultiplayerGameState.hpp"
#include "SettingsState.hpp"



const sf::Time Application::time_per_frame = sf::seconds(1.f / 60.f);

Application::Application()
	: m_window(sf::VideoMode(1344, 960), "States", sf::Style::Close)
	  , m_key_binding_1(1)
	  , m_key_binding_2(2)
	  , m_stack(State::Context(m_window, m_textures, m_fonts, m_music, m_sounds, m_level_manager, m_player_data_manager, m_player_data_manager.GetData().m_player1_keybindings, m_player_data_manager.GetData().m_player2_keybindings))
	  , m_statistics_num_frames(0)
{
	m_window.setKeyRepeatEnabled(false);

	m_fonts.Load(Fonts::Main, "Media/Fonts/Sansation.ttf");
	m_textures.Load(Textures::kTitleScreen, "Media/Textures/TitleScreen.png");
	m_textures.Load(Textures::kButtons, "Media/Textures/Buttons.png");

	m_statistics_text.setFont(m_fonts.Get(Fonts::Main));
	m_statistics_text.setPosition(5.f, 5.f);
	m_statistics_text.setCharacterSize(10u);

	RegisterStates();
	m_stack.PushState(StateID::kTitle);
}

void Application::Run()
{
	sf::Clock clock;
	sf::Time time_since_last_update = sf::Time::Zero;
	while (m_window.isOpen())
	{
		const sf::Time elapsed_time = clock.restart();
		time_since_last_update += elapsed_time;

		while (time_since_last_update > time_per_frame)
		{
			time_since_last_update -= time_per_frame;
			ProcessInput();
			Update(time_per_frame);

			if (m_stack.IsEmpty())
			{
				m_player_data_manager.Save();
				m_window.close();
			}
		}
		UpdateStatistics(elapsed_time);
		Render();
	}
}

void Application::ProcessInput()
{
	sf::Event event{};
	while (m_window.pollEvent(event))
	{
		m_stack.HandleEvent(event);
		if (event.type == sf::Event::Closed)
		{
			m_player_data_manager.Save();
			m_window.close();
		}
	}
}

void Application::Update(const sf::Time delta_time)
{
	m_stack.Update(delta_time);
}

void Application::Render()
{
	m_window.clear();
	m_stack.Draw();

	m_window.setView(m_window.getDefaultView());
	m_window.draw(m_statistics_text);
	m_window.display();
}

void Application::UpdateStatistics(const sf::Time elapsed_time)
{
	m_statistics_update_time += elapsed_time;
	m_statistics_num_frames += 1;

	if (m_statistics_update_time >= sf::seconds(1.0f))
	{
		m_statistics_text.setString(
			"Frames / Second = " + std::to_string(m_statistics_num_frames) + "\n" +
			"Time / Update = " + std::to_string(
				m_statistics_update_time.asMicroseconds() / m_statistics_num_frames) + "us");

		m_statistics_update_time -= sf::seconds(1.0f);
		m_statistics_num_frames = 0;
	}
}

void Application::RegisterStates()
{
	m_stack.RegisterState<TitleState>(StateID::kTitle);
	m_stack.RegisterState<MenuState>(StateID::kMenu);
	m_stack.RegisterState<GameState>(StateID::kGame);
	m_stack.RegisterState<MultiplayerGameState>(StateID::kHostGame, true);
	m_stack.RegisterState<MultiplayerGameState>(StateID::kJoinGame, false);
	m_stack.RegisterState<LevelPauseState>(StateID::kNetworkPause);
	m_stack.RegisterState<LevelPauseState>(StateID::kPause);
	m_stack.RegisterState<SettingsState>(StateID::kSettings);
	m_stack.RegisterState<LevelLoseState>(StateID::kLevelLose);
	m_stack.RegisterState<LevelWinState>(StateID::kLevelWin);
}

