#pragma once
#include <3ds.h>
#include <string>
#include <cstdlib>
#include <cstring>
#include <cstdio>

using namespace std;
class sound
{
public:
	u32 sampleRate;
	u32 dataSize;
	u16 channels;
	u16 bitsPerSample;

	ndspWaveBuf waveBuf;
	u8* data = NULL;
	sound(string path, int channel = 1, bool toloop = true);
	~sound();
	void play();
	void stop();
	int chnl;
};
