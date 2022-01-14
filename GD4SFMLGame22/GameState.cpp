#include "GameState.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

#include "Player.hpp"

GameState::GameState(StateStack& stack, const Context context)
: State(stack, context)
, m_world(*context.window, *context.sounds, *context.level_manager)
, m_player(*context.player)
{
	m_world.SetLoseCallback([this]
	{
		RequestStackPush(StateID::kLevelLose);	
	});

	m_world.SetWinCallback([this]
	{
		RequestStackPush(StateID::kLevelWin);	
	});

	context.music->Play(MusicThemes::kMissionTheme);
}

void GameState::Draw()
{
	m_world.Draw();
}

bool GameState::Update(const sf::Time dt)
{
	m_world.Update(dt);
	CommandQueue& commands = m_world.getCommandQueue();
	m_player.HandleRealtimeInput(commands);
	return true;
}

bool GameState::HandleEvent(const sf::Event& event)
{
	CommandQueue& commands = m_world.getCommandQueue();
	m_player.HandleEvent(event, commands);

	//Escape should bring up the Pause Menu
	if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
	{
		RequestStackPush(StateID::kPause);
	}
	return true;
}
