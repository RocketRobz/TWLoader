#include "sound.h"



sound::sound(string path, int channel, bool toloop)
{

	ndspSetOutputMode(NDSP_OUTPUT_STEREO);
	ndspSetOutputCount(2); // Num of buffers

	// Reading wav file
	FILE* fp = fopen(path.c_str(), "rb");

	if (!fp)
	{
		printf("Could not open the example.wav file.\n");
		return;
	}
	char signature[4];

	fread(signature, 1, 4, fp);

	if (signature[0] != 'R' &&
		signature[1] != 'I' &&
		signature[2] != 'F' &&
		signature[3] != 'F')
	{
		printf("Wrong file format.\n");
		fclose(fp);
		return;
	}

	fseek(fp, 0, SEEK_END);
	dataSize = ftell(fp);
	fseek(fp, 22, SEEK_SET);
	fread(&channels, 2, 1, fp);
	fseek(fp, 24, SEEK_SET);
	fread(&sampleRate, 4, 1, fp);
	fseek(fp, 34, SEEK_SET);
	fread(&bitsPerSample, 2, 1, fp);

	if (dataSize == 0 || (channels != 1 && channels != 2) ||
		(bitsPerSample != 8 && bitsPerSample != 16))
	{
		printf("Corrupted wav file.\n");
		fclose(fp);
		return;
	}

	// Allocating and reading samples
	data = static_cast<u8*>(linearAlloc(dataSize));
	fseek(fp, 44, SEEK_SET);
	fread(data, 1, dataSize, fp);
	fclose(fp);
	dataSize /= 2;
	// Find the right format
	u16 ndspFormat;

	if (bitsPerSample == 8)
	{
		ndspFormat = (channels == 1) ?
			NDSP_FORMAT_MONO_PCM8 :
			NDSP_FORMAT_STEREO_PCM8;
	}
	else
	{
		ndspFormat = (channels == 1) ?
			NDSP_FORMAT_MONO_PCM16 :
			NDSP_FORMAT_STEREO_PCM16;
	}

	ndspChnReset(channel);
	ndspChnSetInterp(channel, NDSP_INTERP_NONE);
	ndspChnSetRate(channel, float(sampleRate));
	ndspChnSetFormat(channel, ndspFormat);

	// Create and play a wav buffer
	std::memset(&waveBuf, 0, sizeof(waveBuf));

	waveBuf.data_vaddr = reinterpret_cast<u32*>(data);
	waveBuf.nsamples = dataSize / (bitsPerSample >> 3);
	waveBuf.looping = toloop;
	waveBuf.status = NDSP_WBUF_FREE;
	chnl = channel;
}


sound::~sound()
{
	waveBuf.data_vaddr = 0;
	waveBuf.nsamples = 0;
	waveBuf.looping = false;
	waveBuf.status = 0;
	ndspChnWaveBufClear(chnl);

	sampleRate = 0;
	dataSize =0;
	channels = 0;
	bitsPerSample = 0;
	if (data) {
		linearFree(data);
	}
}

void sound::play()
{
	if (!data)
		return;
	DSP_FlushDataCache(data, dataSize);
	ndspChnWaveBufAdd(chnl, &waveBuf);
}

void sound::stop()
{
	if (!data)
		return;
	ndspChnWaveBufClear(chnl);
}
