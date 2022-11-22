#pragma once

#include <string>
#include <vector>

#include "cSoundManager.h"

class SoundInfo {

public:

	SoundInfo();
	~SoundInfo();

	void Initialize();
	void Shutdown();

	void CreateChannels();
	void ReadFromFile();
	void LoadSounds();
	void StopAllSounds();

	cSoundManager* soundMan = nullptr;
	cSoundManager* GetSoundManager();

	unsigned int readIndex = 0;

	std::vector<std::string> soundFiles;
};