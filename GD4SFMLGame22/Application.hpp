#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Time.hpp>

#include "MusicPlayer.hpp"
#include "Player.hpp"
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "StateStack.hpp"

#include "LevelManager.hpp"
#include "PlayerDataManager.hpp"


class Application
{
public:
	Application();
	void Run();

private:
	void ProcessInput();
	void Update(sf::Time delta_time);
	void Render();
	void UpdateStatistics(sf::Time elapsed_time);
	void RegisterStates();

	sf::RenderWindow m_window;

	KeyBinding m_key_binding_1;
	KeyBinding m_key_binding_2;

	TextureHolder m_textures;
	FontHolder m_fonts;

	MusicPlayer m_music;
	SoundPlayer m_sounds;

	LevelManager m_level_manager;
	PlayerDataManager m_player_data_manager;

	Context m_context;
	StateStack m_stack;

	sf::Text m_statistics_text;
	sf::Time m_statistics_update_time;

	std::size_t m_statistics_num_frames;
	static const sf::Time time_per_frame;
};
