#pragma once
#include <3ds.h>
#include <string>

class sound
{
public:
	sound(const std::string& path, int channel = 1, bool toloop = true);
	~sound();
	void play();
	void stop();

private:
	u32 sampleRate;
	u32 dataSize;
	u16 channels;
	u16 bitsPerSample;

	ndspWaveBuf waveBuf;
	u8* data = NULL;
	int chnl;
};
