#pragma once
#include "StateID.hpp"
#include "ResourceIdentifiers.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include <memory>

#include "KeyBinding.hpp"
#include "LevelManager.hpp"
#include "MusicPlayer.hpp"
#include "PlayerDataManager.hpp"
#include "SoundPlayer.hpp"

namespace sf
{
	class RenderWindow;
}

class StateStack;
class Player;

class State
{
public:
	typedef std::unique_ptr<State> Ptr;

	struct Context
	{
		Context(sf::RenderWindow& window, TextureHolder& textures, FontHolder& fonts, MusicPlayer& music, SoundPlayer& sounds, LevelManager& level_manager, PlayerDataManager& player_data_manager, KeyBinding& keys1, KeyBinding& keys2);
		sf::RenderWindow* m_window;
		TextureHolder* m_textures;
		FontHolder* m_fonts;
		MusicPlayer* m_music;
		SoundPlayer* m_sounds;
		LevelManager* m_level_manager;
		PlayerDataManager* m_player_data_manager;
		KeyBinding* m_keys1;
		KeyBinding* m_keys2;
	};

public:
	State(StateStack& stack, Context context);
	virtual ~State();
	virtual void Draw() = 0;
	virtual bool Update(sf::Time dt) = 0;
	virtual bool HandleEvent(const sf::Event& event) = 0;
	virtual void OnStackPopped();
	void RequestStackPush(StateID state_id) const;

protected:
	void RequestStackPop() const;
	void RequestStackClear() const;
	Context GetContext() const;

private:
	StateStack* m_stack;
	Context m_context;
};