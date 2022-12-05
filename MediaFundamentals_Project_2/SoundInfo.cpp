#include "SoundInfo.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <iostream>
#include <fstream>

#include <FMOD/fmod.hpp>

char classic_rock_url[] = "http://78.129.202.200:8040";
char celtic_url[] = "http://192.111.140.11:8058";
char country_url[] = "http://216.235.89.162/1976_128.mp3";
char bbc_url[] = "http://stream.live.vc.bbcmedia.co.uk/bbc_radio_one";

SoundInfo::SoundInfo() {
	soundMan = new cSoundManager();
}

SoundInfo::~SoundInfo() {

}

void SoundInfo::Initialize(GLFWwindow* window, const char* glsl_version) {

	// Init dear ImGUI	
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	ImGui::StyleColorsDark();		//dark theme

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

void SoundInfo::LoadInternetSounds(glm::vec3 vecPosition) {

	soundMan->LoadInternetSound("bbc_radio", bbc_url, FMOD_3D | FMOD_CREATESTREAM | FMOD_NONBLOCKING);
	soundMan->LoadInternetSound("classic_rock", classic_rock_url, FMOD_3D | FMOD_CREATESTREAM | FMOD_NONBLOCKING);
	soundMan->LoadInternetSound("country", country_url, FMOD_3D | FMOD_CREATESTREAM | FMOD_NONBLOCKING);
	soundMan->LoadInternetSound("celtic", celtic_url, FMOD_3D | FMOD_CREATESTREAM | FMOD_NONBLOCKING);

	soundURLs.push_back("bbc_radio");
	soundURLs.push_back("classic_rock");
	soundURLs.push_back("country");
	soundURLs.push_back("celtic");

	const auto radio = soundMan->inetSounds[soundURLs[currentURL]];
	result = radio->getOpenState(&openstate, &percentage, &is_starving, nullptr);
	assert(!result);

	if (channel[0]) {
		while (radio->getTag(nullptr, -1, &tag) == FMOD_OK)
		{
			if (tag.datatype == FMOD_TAGDATATYPE_STRING)
			{
				printf(tag_string[tag_index], "%s = %s (%d bytes)",
					tag.name, static_cast<char*>(tag.data), tag.datalen);

				tag_index = (tag_index + 1) % NUMBER_OF_TAGS;
			}
			else
			{
				float frequency = *static_cast<float*>(tag.data);
				result = channel[0]->setFrequency(frequency);
				assert(!result);
				std::cout << frequency;
			}
			
		}

		result = channel[0]->getPaused(&is_paused);
		assert(!result);
		result = channel[0]->isPlaying(&is_playing);
		assert(!result);
		result = channel[0]->getPosition(&position, FMOD_TIMEUNIT_MS);
		assert(!result);

		result = channel[0]->setMute(is_starving);
		assert(!result);
	}
	else
	{
		soundMan->PlayInternetSound(soundURLs[currentURL], vecPosition, 0.25f, &channel[0]);
		channel[0]->setVolume(25.f);
	}

	if (openstate == FMOD_OPENSTATE_CONNECTING)
	{
		current_state = "Connecting...";
	}
	else if (openstate == FMOD_OPENSTATE_BUFFERING)
	{
		current_state = "Buffering...";
	}
	else if (is_paused)
	{
		current_state = "Paused";
	}
	else
	{
		current_state = "Playing";
	}
}

void SoundInfo::LoadGui(glm::vec3 position) {

	// Start a new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Internet Radio");
	ImGui::Text("Currently attached to: tesla_cybertruck_mesh");
	ImGui::Text("Now playing: ");
	ImGui::Text(soundURLs[currentURL].c_str());
	if (ImGui::Button("Pause")) {
		channel[0]->setPaused(!is_paused);
		is_paused = !is_paused;
	}
	if (ImGui::Button("Previous")) {
		currentURL--;
		if (currentURL < 0) {
			currentURL = soundURLs.size() - 1;
		}
		channel[0]->setPaused(true);
		soundMan->PlayInternetSound(soundURLs[currentURL], position, 0.25f, &channel[0]);
	}
	ImGui::SameLine();
	if (ImGui::Button("Next")) {
		currentURL++;
		if (currentURL > soundURLs.size() - 1) {
			currentURL = 0;
		}
		channel[0]->setPaused(true);
		soundMan->PlayInternetSound(soundURLs[currentURL], position, 0.25f, &channel[0]);
	}

	ImGui::Text("Time: ");
	ImGui::SameLine();
	std::string time0 = std::to_string(this->position / 1000 / 60);
	ImGui::Text(time0.c_str());
	ImGui::SameLine();
	ImGui::Text(":");
	ImGui::SameLine();
	std::string time1 = std::to_string(this->position / 100 % 60);
	ImGui::Text(time1.c_str());
	ImGui::SameLine();
	ImGui::Text(":");
	ImGui::SameLine();
	std::string time2 = std::to_string(this->position / 10 % 100);
	ImGui::Text(time2.c_str());

	ImGui::Text("Current state: ");
	ImGui::SameLine();
	ImGui::Text(current_state);

	ImGui::Text("Buffer percentage: ");
	ImGui::SameLine();
	std::string temp = std::to_string(percentage);
	ImGui::Text(temp.c_str());

	ImGui::End();

	// Sound Position
	soundMan->UpdateSoundPosition(channel[0], position);

	// Render imgui stuff to screen
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// Gracefully close everything down
void SoundInfo::Shutdown() {
	soundFiles.clear();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

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