#include "SoundInfo.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <iostream>
#include <fstream>

#include <FMOD/fmod.hpp>

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
	ReadFromURLFile();
	CreateChannels();
	CreateDSPs();
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

void SoundInfo::ReadFromURLFile() {
	std::ifstream readFile("readURL.txt");
	std::string input;

	while (readFile >> input) {
		soundURL.push_back(input);
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

void SoundInfo::CreateDSPs() {
	soundMan->CreateDSPEffect("PitchShift", FMOD_DSP_TYPE_PITCHSHIFT, 2.f);
	soundMan->CreateDSPEffect("Distortion", FMOD_DSP_TYPE_DISTORTION, 1.f);
	soundMan->CreateDSPEffect("Echo", FMOD_DSP_TYPE_ECHO, 250.f);
	soundMan->CreateDSPEffect("Reverb", FMOD_DSP_TYPE_SFXREVERB, 500000000.f * 500000000.f);
	soundMan->CreateDSPEffect("Fader", FMOD_DSP_TYPE_FADER, 100.f);
	soundMan->CreateDSPEffect("Chorus", FMOD_DSP_TYPE_CHORUS, 500.f);
	soundMan->CreateDSPEffect("Delay", FMOD_DSP_TYPE_DELAY, 10000.f);
	soundMan->CreateDSPEffect("Tremolo", FMOD_DSP_TYPE_TREMOLO, 1.f);
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

	soundMan->LoadInternetSound("bbc_radio", soundURL[0], FMOD_3D | FMOD_CREATESTREAM | FMOD_NONBLOCKING);
	soundMan->LoadInternetSound("classic_rock", soundURL[1], FMOD_3D | FMOD_CREATESTREAM | FMOD_NONBLOCKING);
	soundMan->LoadInternetSound("country", soundURL[2], FMOD_3D | FMOD_CREATESTREAM | FMOD_NONBLOCKING);
	soundMan->LoadInternetSound("celtic", soundURL[3], FMOD_3D | FMOD_CREATESTREAM | FMOD_NONBLOCKING);

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

	// The actual gui
	ImGui::Begin("Internet Radio");
	ImGui::Text("Currently attached to: tesla_cybertruck_mesh");

	ImGui::Text(" ");
	ImGui::Text("Now playing: ");
	ImGui::Text(soundURLs[currentURL].c_str());
	if (ImGui::Button("Pause")) {
		channel[0]->setPaused(!is_paused);
		is_paused = !is_paused;
	}
	ImGui::Text(" ");
	ImGui::Text("Station Controls: ");
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

	ImGui::Text(" ");
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

	ImGui::Text("Buffer percentage: ");
	ImGui::SameLine();
	std::string temp = std::to_string(percentage);
	ImGui::Text(temp.c_str());

	ImGui::Text("Current state: ");
	ImGui::SameLine();
	ImGui::Text(current_state);

	ImGui::Text(" ");
	float volume = 0.f;
	soundMan->GetVolume(channel[0], &volume);
	volume *= 100;
	ImGui::SliderFloat("Radio Volume", &volume, 0.f, 100.f, "%.0f");
	volume /= 100;
	soundMan->SetVolume(channel[0], volume);

	ImGui::Text(" ");
	ImGui::Text("DSP Effects: ");
	if (ImGui::Button("Pitch Shift")) {
		soundMan->AddDSPEffect(channel[0], "PitchShift");
	}
	ImGui::SameLine();
	if (ImGui::Button("Distortion")) {
		soundMan->AddDSPEffect(channel[0], "Distortion");
	}
	ImGui::SameLine();
	if (ImGui::Button("Echo")) {
		soundMan->AddDSPEffect(channel[0], "Echo");
	}
	ImGui::SameLine();
	if (ImGui::Button("Reverb")) {
		soundMan->AddDSPEffect(channel[0], "Reverb");
	}
	if (ImGui::Button("Fader")) {
		soundMan->AddDSPEffect(channel[0], "Fader");
	}
	ImGui::SameLine();
	if (ImGui::Button("Chorus")) {
		soundMan->AddDSPEffect(channel[0], "Chorus");
	}
	ImGui::SameLine();
	if (ImGui::Button("Delay")) {
		soundMan->AddDSPEffect(channel[0], "Delay");
	}
	ImGui::SameLine();
	if (ImGui::Button("Tremolo")) {
		soundMan->AddDSPEffect(channel[0], "Tremolo");
	}

	ImGui::Text(" ");
	ImGui::Text("Press 'F' to teleport listener to the mesh with attached radio.");
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
	soundURLs.clear();

	for (int i = 0; i < NUM_OF_SOUNDS; i++) {
		sound[i]->release();
		sound[i] = nullptr;
		delete sound[i];
	}

	for (int i = 0; i < NUM_OF_SOUNDS; i++) {
		channel[i] = nullptr;
		delete channel[i];
	}

	soundMan->ShutDown();
	soundMan = nullptr;
	delete soundMan;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
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
	return this->soundMan;
}