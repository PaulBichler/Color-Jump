#include "MusicPlayer.hpp"


MusicPlayer::MusicPlayer() : m_volume(10.f)
{
	m_fileNames[MusicThemes::kMenuTheme] = "Media/Music/FallenLeaves.ogg";
	m_fileNames[MusicThemes::kMissionTheme] = "Media/Music/ByeByeBrain.ogg";
}

void MusicPlayer::Play(const MusicThemes theme)
{
	const std::string filename = m_fileNames[theme];

	if (!m_music.openFromFile(filename))
		throw std::runtime_error("Music " + filename + " could not be loaded.");

	m_music.setVolume(m_volume);
	m_music.setLoop(true);
	m_music.play();
}

void MusicPlayer::Stop()
{
	m_music.stop();
}

void MusicPlayer::SetVolume(const float volume)
{
	m_volume = volume;
}

void MusicPlayer::SetPaused(const bool paused)
{
	if (paused)
		m_music.pause();
	else
		m_music.play();
}
