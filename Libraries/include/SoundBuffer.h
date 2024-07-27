#ifndef SOUND_BUFFER_H
#define SOUND_BUFFER_H

#include <AL\al.h>
#include <vector>

struct Sound 
{
	const char* filename;
	uint32_t sound;
};

class SoundBuffer
{
public:
	static SoundBuffer* get();

	Sound addSoundEffect(const char* filename);
	bool removeSoundEffect(const ALuint& buffer);

private:
	SoundBuffer();
	~SoundBuffer();

	std::vector<ALuint> p_SoundEffectBuffers;
};

#endif