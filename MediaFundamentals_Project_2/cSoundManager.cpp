#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "cSoundManager.h"

cSoundManager::cSoundManager() {

}

cSoundManager::~cSoundManager() {

}

bool cSoundManager::Initialize() {

	last_result = System_Create(&fmod_sys);
	if (last_result != FMOD_OK) {
		std::cout << "FMOD system creation exit with error: " << FMOD_ErrorString(last_result) << std::endl;
		return false;
	}

	last_result = fmod_sys->init(MAX_CHANNELS, FMOD_INIT_NORMAL, NULL);
	if (last_result != FMOD_OK) {
		std::cout << "FMOD system init exit with error: " << FMOD_ErrorString(last_result) << std::endl;
		return false;
	}

	return true;
}

void cSoundManager::ShutDown() {

	for (auto i = dsps.begin(); i != dsps.end(); i++) {
		i->second->release();
	}
	dsps.clear();

	for (auto i = sounds.begin(); i != sounds.end(); i++) {
		i->second->release();
	}
	sounds.clear();

	for (auto i = channel_groups.begin(); i != channel_groups.end(); i++) {
		i->second->current_grp->release();
	}
	channel_groups.clear();

	if (fmod_sys) {
		fmod_sys->release();
		fmod_sys = nullptr;
		delete fmod_sys;
	}
}

int cSoundManager::CreateChannelGroup(std::string name) {
	FMOD::ChannelGroup* channel_group;

	last_result = fmod_sys->createChannelGroup(name.c_str(), &channel_group);
	if (last_result != FMOD_OK) {
		std::cout << "FMOD creating channel(s) exit with error: " << FMOD_ErrorString(last_result) << std::endl;
		return 1;
	}

	auto* temp = new ChannelGroup();
	temp->current_grp = channel_group;

	channel_groups.try_emplace(name, temp);
	return 0;
}

int cSoundManager::SetParentChannel(const std::string& child_name, const std::string& parent_name) {

	const auto child_group = channel_groups.find(child_name);
	const auto parent_group = channel_groups.find(parent_name);

	if (child_group == channel_groups.end() || parent_group == channel_groups.end())
	{
		return 1;
	}

	last_result = parent_group->second->current_grp->addGroup(child_group->second->current_grp);
	if (last_result != FMOD_OK) {
		std::cout << "FMOD setting parent channel exit with error: " << FMOD_ErrorString(last_result) << std::endl;
		return 1;
	}

	return 0;
}

int cSoundManager::LoadSounds(const std::string& name, const std::string& path, const int mode) {

	FMOD::Sound* sound;
	last_result = fmod_sys->createSound(path.c_str(), mode, nullptr, &sound);
	if (last_result != FMOD_OK) {
		std::cout << "FMOD creating sound(s) exit with error: " << FMOD_ErrorString(last_result) << std::endl;
		return 1;
	}

	sounds.try_emplace(name, sound);

	return 0;
}


int cSoundManager::PlaySounds(const std::string& sound_name, const std::string& channel_group_name) {

	const auto sound_iterator = sounds.find(sound_name);
	const auto channel_group_iterator = channel_groups.find(channel_group_name);

	if (sound_iterator == sounds.end() || channel_group_iterator == channel_groups.end())
	{
		return 1;
	}

	last_result = fmod_sys->playSound(sound_iterator->second, channel_group_iterator->second->current_grp, true, &channel);
	if (last_result != FMOD_OK) {
		std::cout << "FMOD playing sound(s) exit with error: " << FMOD_ErrorString(last_result) << std::endl;
		return 1;
	}
	std::cout << "Music playing, yes?";

	last_result = (*channel).setPaused(false);
	if (last_result != FMOD_OK) {
		std::cout << "FMOD setting sound status exit with error: " << FMOD_ErrorString(last_result) << std::endl;
		return 1;
	}

	return 0;
}

int cSoundManager::PauseSounds(const std::string& channel_group_name, bool paused) {
	const auto channel_group_iterator = channel_groups.find(channel_group_name);

	if (channel_group_iterator == channel_groups.end())
	{
		return 1;
	}
	last_result = (*channel).setPaused(paused);
	if (last_result != FMOD_OK) {

		return 1;
	}
}

int cSoundManager::SetChannelGroupVolume(const std::string& name, float volume)
{
	const auto temp = channel_groups.find(name);
	if (temp == channel_groups.end())
	{
		std::cout << "FMOD could not locate channel group.";
		return 1;
	}

	last_result = temp->second->current_grp->setVolume(volume);
	if (last_result != FMOD_OK) {
		std::cout << "FMOD setting sound volume exit with error: " << FMOD_ErrorString(last_result) << std::endl;
		return 1;
	}

	return 0;
}

int cSoundManager::FetchChannelGroup(const std::string& name, ChannelGroup** channel_group)
{
	const auto iterator = channel_groups.find(name);
	if (iterator == channel_groups.end())
	{
		return 1;
	}

	*channel_group = iterator->second;

	return 0;
}

float cSoundManager::GetVolume(const std::string& name, float* volume) {
	const auto iterator = channel_groups.find(name);
	if (iterator == channel_groups.end())
	{
		return 1;
	}

	return iterator->second->current_grp->getVolume(volume);
}

float cSoundManager::SetVolume(const std::string& name, float volume) {
	const auto iterator = channel_groups.find(name);
	if (iterator == channel_groups.end())
	{
		return 1;
	}

	return iterator->second->current_grp->setVolume(volume);
}

int cSoundManager::GetChannelGroupStatus(const std::string& name, bool* enabled) {
	const auto iterator = channel_groups.find(name);
	if (iterator == channel_groups.end())
	{
		return 1;
	}

	if (!iterator->second->current_grp->getMute(enabled))
	{
		return 1;
	}

	*enabled = !(*enabled);

	return 0;
}

int cSoundManager::SetChannelGroupStatus(const std::string& name, bool enabled) {
	const auto iterator = channel_groups.find(name);
	if (iterator == channel_groups.end())
	{
		return 1;
	}

	if (!iterator->second->current_grp->setMute(enabled))
	{
		return 1;
	}

	return 0;
}

int cSoundManager::SetChannelPan(const std::string& name, const float pan) {
	const auto iterator = channel_groups.find(name);
	if (iterator == channel_groups.end())
	{
		return 1;
	}

	if (!iterator->second->current_grp->setPan(pan))
	{
		return 1;
	}

	return 0;
}

int cSoundManager::CreateDSPEffect(const std::string& name, FMOD_DSP_TYPE dsp_type, const float value) {
	FMOD::DSP* dsp;

	//figure out the kind we are creating

	if (fmod_sys->createDSPByType(dsp_type, &dsp)) {
		return 1;
	}

	if (dsp->setParameterFloat(0, value))
	{
		return 1;
	}

	dsps.try_emplace(name, dsp);
	return 0;
}

int cSoundManager::GetDSPEffect(const std::string& name, FMOD::DSP** dsp) {
	const auto dsp_effect_iterator = dsps.find(name);
	if (dsp_effect_iterator == dsps.end())
	{
		return 1;
	}

	*dsp = dsp_effect_iterator->second;

	return 0;
}

int cSoundManager::AddDSPEffect(const std::string& channel_group_name, const std::string& effect_name) {

	const auto channel_group_iterator = channel_groups.find(channel_group_name);
	const auto dsp_effect_iterator = dsps.find(effect_name);
	if (channel_group_iterator == channel_groups.end() || dsp_effect_iterator == dsps.end())
	{
		return 1;
	}

	int num_dsp;
	channel_group_iterator->second->current_grp->getNumDSPs(&num_dsp);


	channel_group_iterator->second->current_grp->addDSP(num_dsp, dsp_effect_iterator->second);

	return 0;
}

bool cSoundManager::Tick(const glm::vec3& camera_position)
{
	assert(fmod_sys && "no system object");

	//update listener to camera position
	FMOD_VECTOR fmod_camera_position;
	fmod_camera_position.x = camera_position.x;
	fmod_camera_position.y = camera_position.y;
	fmod_camera_position.z = camera_position.z;

	fmod_sys->set3DListenerAttributes(0, &fmod_camera_position, nullptr, nullptr, nullptr);

	return fmod_sys->update();
}

bool cSoundManager::SetListenerPosition(const glm::vec3 position)
{
	FMOD_VECTOR fmod_position;
	fmod_position.x = position.x;
	fmod_position.y = position.y;
	fmod_position.z = position.z;

	return fmod_sys->set3DListenerAttributes(0, &fmod_position, nullptr, nullptr, nullptr);
}

bool cSoundManager::PlaySounds(const std::string& sound_name, glm::vec3 position, float max_distance, FMOD::Channel** channel)
{
	assert(fmod_sys && "no system object");

	assert(sounds.find(sound_name) != sounds.end() && "sound not found");
	const auto sound = sounds.find(sound_name);
	if (sound == sounds.end())
	{
		return false;
	}

	last_result = fmod_sys->playSound(sound->second, nullptr, true, channel);
	if (last_result != FMOD_OK) {
		std::cout << "FMOD playing sound(s) exit with error: " << FMOD_ErrorString(last_result) << std::endl;
		return 1;
	}

	FMOD_VECTOR fmod_sound_position;
	fmod_sound_position.x = position.x;
	fmod_sound_position.y = position.y;
	fmod_sound_position.z = position.z;

	(*channel)->set3DAttributes(&fmod_sound_position, nullptr);

	//min distance to hear @ max volume
	//max distance where the attenuation stops
	(*channel)->set3DMinMaxDistance(max_distance, 10000.0f);

	(*channel)->setPaused(false);

	return true;
}

bool cSoundManager::UpdateSoundPosition(FMOD::Channel* channel, const glm::vec3 position)
{
	FMOD_VECTOR fmod_position;
	fmod_position.x = position.x;
	fmod_position.y = position.y;
	fmod_position.z = position.z;
	return channel->set3DAttributes(&fmod_position, nullptr);
}

bool cSoundManager::UpdateVolume(FMOD::Channel* channel, const float new_volume)
{
	return channel->setVolume(new_volume);
}