#pragma once
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/Music.hpp>
#include <map>

class AudioManager
{
private:
	sf::Music music;
	sf::Sound sound;
	std::map<std::string, sf::SoundBuffer> mapSoundBuffers;
	std::map<std::string, const char*> mapMusicPath;

public:
	void LoadSoundBuffer(std::string strID, const char* strPath);
	void LoadMusicPath(std::string strID, const char* strPath);

	void PlayMusic(std::string strID, bool bRepeat = false);
	void PlaySound(std::string strID);
	void PauseMusic();
	void PauseSound();
	void ResumeMusic();
	void ResumeSound();
	void StopMusic();
	

	bool IsPlayingMusic();
};

