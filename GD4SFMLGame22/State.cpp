#include "State.hpp"

#include "StateStack.hpp"

State::Context::Context(sf::RenderWindow& window, TextureHolder& textures, FontHolder& fonts,
                        MusicPlayer& music, SoundPlayer& sounds, LevelManager& level_manager, PlayerDataManager& player_data_manager,
                        KeyBinding& keys1, KeyBinding& keys2)
	: m_window(&window)
	  , m_textures(&textures)
	  , m_fonts(&fonts)
	  , m_music(&music)
	  , m_sounds(&sounds)
	  , m_level_manager(&level_manager)
	  , m_player_data_manager(&player_data_manager)
	  , m_keys1(&keys1)
	  , m_keys2(&keys2)
{
}

State::State(StateStack& stack, const Context context)
	: m_stack(&stack)
	  , m_context(context)
{
}

State::~State() = default;


void State::RequestStackPush(const StateID state_id) const
{
	if (true)
	{
		
	}

	m_stack->PushState(state_id);
}

void State::RequestStackPop() const
{
	m_stack->PopState();
}

void State::RequestStackClear() const
{
	m_stack->ClearStates();
}

State::Context State::GetContext() const
{
	return m_context;
}
