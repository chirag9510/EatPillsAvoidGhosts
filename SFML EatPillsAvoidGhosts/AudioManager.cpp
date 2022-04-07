#include "AudioManager.h"
#include <iostream>

void AudioManager::LoadSoundBuffer(std::string strID, const char* szPath)
{
	if(!mapSoundBuffers[strID].loadFromFile(szPath))
		std::cerr << "LoadSoundBuffer() failed ID : " << strID << std::endl;
}

void AudioManager::LoadMusicPath(std::string strID, const char* szPath)
{
	mapMusicPath[strID] = szPath;
}

void AudioManager::PlayMusic(std::string strID, bool bRepeat)
{
	if (!music.openFromFile(mapMusicPath[strID]))
	{
		std::cerr << "PlayMusic() failed ID : " << strID << std::endl;
		return;
 	}
	else
	{
		music.setLoop(bRepeat);
		music.play();
	}
}

void AudioManager::PlaySound(std::string strID)
{
	sound.setBuffer(mapSoundBuffers[strID]);
	sound.play();
}

void AudioManager::PauseMusic()
{
	music.pause();
}

void AudioManager::PauseSound()
{
	sound.pause();
}

void AudioManager::ResumeMusic()
{
	music.play();
}

void AudioManager::ResumeSound()
{
	sound.play();
}


void AudioManager::StopMusic()
{
	music.stop();
}

bool AudioManager::IsPlayingMusic()
{
	return music.getStatus() == sf::Music::Playing;
}