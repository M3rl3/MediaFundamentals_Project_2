#pragma once

#include "OpenGL.h"

#include <string>
#include <vector>

#include "cSoundManager.h"

#define NUM_OF_SOUNDS 3

#define NUMBER_OF_TAGS 10

class SoundInfo {

	FMOD_TAG tag;

	FMOD_OPENSTATE openstate = FMOD_OPENSTATE_READY;
	int tag_index = 0;
	char tag_string[NUMBER_OF_TAGS][128] = { 0 };

	unsigned int position = 0;
	unsigned int percentage = 0;
	bool is_playing = false;
	bool is_paused = false;
	bool is_starving = false;
	const char* current_state = "Stopped";

public:

	SoundInfo();
	~SoundInfo();

	void Initialize(GLFWwindow* window, const char* glsl_version);
	void Shutdown();

	void CreateChannels();
	void CreateDSPs();
	void ReadFromFile();
	void LoadSounds();
	void LoadInternetSounds(glm::vec3 vecPosition);
	void LoadGui(glm::vec3 position);
	void StopAllSounds();

	cSoundManager* soundMan = nullptr;
	cSoundManager* GetSoundManager();

	FMOD::Sound* sound[NUM_OF_SOUNDS];
	FMOD::Channel* channel[NUM_OF_SOUNDS];

	unsigned int readIndex = 0;

	FMOD_RESULT result = FMOD_OK;

	std::vector<std::string> soundFiles;
	std::vector<std::string> soundURLs;

	int currentURL = 0;
};