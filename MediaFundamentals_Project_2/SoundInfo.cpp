#include "SoundInfo.h"

#include <iostream>
#include <fstream>

#include <FMOD/fmod.hpp>

SoundInfo::SoundInfo() {
	soundMan = new cSoundManager();
}

SoundInfo::~SoundInfo() {

}

void SoundInfo::Initialize() {
	if (!soundMan->Initialize()) {
		std::cout << "Error: Failed to initalize FMOD." << std::endl;
	}

	ReadFromFile();
	CreateChannels();
	LoadSounds();
}

void SoundInfo::ReadFromFile() {
	std::ifstream readFile("soundFiles.txt");
	std::string input;

	while (readFile >> input) {
		soundFiles.push_back(input);
		readIndex++;
	}
}

void SoundInfo::CreateChannels() {
	soundMan->CreateChannelGroup("master");
	soundMan->CreateChannelGroup("sounds");
	soundMan->CreateChannelGroup("sfx");
			
	soundMan->SetParentChannel("sounds", "master");
	soundMan->SetParentChannel("sfx", "master");
	soundMan->SetChannelGroupVolume("sounds", 0.5f);
	soundMan->SetChannelGroupVolume("sfx", 0.5f);
}

void SoundInfo::LoadSounds() {
	constexpr int flags = FMOD_DEFAULT | FMOD_3D | FMOD_LOOP_NORMAL;
	
	soundMan->LoadSounds("ambulance", soundFiles[0], flags);
	soundMan->LoadSounds("car_horn", soundFiles[1], flags);
	soundMan->LoadSounds("engine", soundFiles[2], flags);
	soundMan->LoadSounds("truck_horn", soundFiles[3], flags);
	soundMan->LoadSounds("chicken", soundFiles[4], flags);
	soundMan->LoadSounds("my_dark_disquiet", soundFiles[5], flags);
}

void SoundInfo::Shutdown() {
	soundFiles.clear();

	soundMan->ShutDown();
	delete soundMan;	
}

void SoundInfo::StopAllSounds() {
	cSoundManager::ChannelGroup* channelGroup0;
	soundMan->FetchChannelGroup("master", &channelGroup0);
	
	cSoundManager::ChannelGroup* channelGroup1;
	soundMan->FetchChannelGroup("sounds", &channelGroup1);

	cSoundManager::ChannelGroup* channelGroup2;
	soundMan->FetchChannelGroup("sfx", &channelGroup2);

	channelGroup1->current_grp->stop();
	channelGroup2->current_grp->stop();
	channelGroup0->current_grp->stop();
}

cSoundManager* SoundInfo::GetSoundManager() {
	return soundMan;
}