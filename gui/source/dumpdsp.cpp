#include <stdio.h>
#include <string.h>
#include <3ds.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "sha256.h"
#define NB_TITLES (sizeof(titles)/sizeof(TitleInfo))

typedef struct {
	u64 titleid;
	char name[50];
} TitleInfo;

TitleInfo titles[] = {

{ 0x4003000008f02LL, "home_menu_usa"},
{ 0x4003000008202LL, "home_menu_jpn"},
{ 0x4003000009802LL, "home_menu_eur"},
{ 0x400300000A102LL, "home_menu_chn"},
{ 0x400300000A902LL, "home_menu_kor"},
{ 0x400300000B102LL, "home_menu_twn"},

};

const void *memmem(
  const void *haystack, size_t haystacklen, 
  const void *needle,   size_t needlelen )
{
  // Sanity check
  if (needlelen > haystacklen) return NULL;

  // Void is useless -- we must treat our data as bytes (== unsigned chars)
  typedef const unsigned char* p;

  // We'll stop searching at the last possible position for a match, 
  // which is haystack[ haystacklen - needlelen + 1 ]
  haystacklen -= needlelen - 1;

  while (haystacklen)
  {
    // Find the first byte in a potential match
    p z = (p)memchr( haystack, *(p)needle, haystacklen ); 
    if (!z) return NULL;

    // Is there enough space for there to actually be a match?
    ptrdiff_t delta = z - (p)haystack;
    ptrdiff_t remaining = (ptrdiff_t)haystacklen - delta;
    if (remaining < 1) return NULL;

    // Advance our pointer and update the amount of haystack remaining
    haystacklen -= delta;
    haystack = z;

    // Did we find a match?
    if (!memcmp( haystack, needle, needlelen )) return haystack;
    
    // Ready for next loop
    haystack = (p)haystack + 1;
    haystacklen -= 1;
  }
  return NULL;
}

// decompression code stolen from ctrtool

u32 getle32(const u8* p)
{
	return (p[0]<<0) | (p[1]<<8) | (p[2]<<16) | (p[3]<<24);
}

u32 lzss_get_decompressed_size(u8* compressed, u32 compressedsize)
{
	u8* footer = compressed + compressedsize - 8;

	u32 originalbottom = getle32(footer+4);

	return originalbottom + compressedsize;
}

int lzss_decompress(u8* compressed, u32 compressedsize, u8* decompressed, u32 decompressedsize)
{
	u8* footer = compressed + compressedsize - 8;
	u32 buffertopandbottom = getle32(footer+0);
	//u32 originalbottom = getle32(footer+4);
	u32 i, j;
	u32 out = decompressedsize;
	u32 index = compressedsize - ((buffertopandbottom>>24)&0xFF);
	u32 segmentoffset;
	u32 segmentsize;
	u8 control;
	u32 stopindex = compressedsize - (buffertopandbottom&0xFFFFFF);

	memset(decompressed, 0, decompressedsize);
	memcpy(decompressed, compressed, compressedsize);

	
	while(index > stopindex)
	{
		control = compressed[--index];
		

		for(i=0; i<8; i++)
		{
			if (index <= stopindex)
				break;

			if (index <= 0)
				break;

			if (out <= 0)
				break;

			if (control & 0x80)
			{
				if (index < 2)
				{
					// fprintf(stderr, "Error, compression out of bounds\n");
					goto clean;
				}

				index -= 2;

				segmentoffset = compressed[index] | (compressed[index+1]<<8);
				segmentsize = ((segmentoffset >> 12)&15)+3;
				segmentoffset &= 0x0FFF;
				segmentoffset += 2;

				
				if (out < segmentsize)
				{
					// fprintf(stderr, "Error, compression out of bounds\n");
					goto clean;
				}

				for(j=0; j<segmentsize; j++)
				{
					u8 data;
					
					if (out+segmentoffset >= decompressedsize)
					{
						// fprintf(stderr, "Error, compression out of bounds\n");
						goto clean;
					}

					data  = decompressed[out+segmentoffset];
					decompressed[--out] = data;
				}
			}
			else
			{
				if (out < 1)
				{
					// fprintf(stderr, "Error, compression out of bounds\n");
					goto clean;
				}
				decompressed[--out] = compressed[--index];
			}

			control <<= 1;
		}
	}

	return 0;
	
	clean:
	return -1;
}


Result openCode(Handle* out, u64 tid, u8 mediatype)
{
	if(!out)return -1;

	u32 archivePath[] = {tid & 0xFFFFFFFF, (tid >> 32) & 0xFFFFFFFF, mediatype, 0x00000000};
	static const u32 filePath[] = {0x00000000, 0x00000000, 0x00000002, 0x646F632E, 0x00000065};

	return FSUSER_OpenFileDirectly(out, (FS_ArchiveID)0x2345678a, (FS_Path){PATH_BINARY, 0x10, (u8*)archivePath}, (FS_Path){PATH_BINARY, 0x14, (u8*)filePath}, FS_OPEN_READ, 0);
}

u32 u8to32(u8 *input){  //workaround for weird < 9.0 bug in built-in sha function
	return *(input+0) | *(input+1)<<8 | *(input+2)<<16 | *(input+3)<<24;
}

int sha_quick(uint8_t *dest, uint8_t *src, size_t src_len) //thanks to wolfvak https://github.com/Wolfvak/makefirm
{
    SHA256_CTX *ctx = (SHA256_CTX*)malloc(sizeof(SHA256_CTX));
    if (!ctx) return 1;
    sha256_init(ctx);
    sha256_update(ctx, src, src_len);
    sha256_final(ctx, dest);
    free(ctx);
    return 0;
}

int checkHashes(u8 *base){
		
	u8 *pos = base + 0x120;
	u8 *dsp_section_offset = NULL;
	u32 dsp_hash_address = 0;
	u8 *dsp_hash_offset = NULL;
	u32 dsp_section_size = 0;
	u8 sha256[0x20];
	int fail=0;

	for(int i=0 ; i<10 ; i++){  //there are a max of ten sections for dsp firm. i've only observed a usage of exactly 5 in the wild
		
		dsp_section_size = u8to32(pos + 0x8);
		dsp_hash_address = u8to32(pos);
		dsp_section_offset = (dsp_hash_address + base);
		dsp_hash_offset = (pos + 0x10);
		memset(sha256, 0, 0x20);
		
		if(dsp_section_size==0) break; //don't hash an empty section header.
		
		//res = FSUSER_UpdateSha256Context(dsp_section_offset, dsp_section_size, sha256); //couldn't use this because buggy on older firms, damn.  
		sha_quick(sha256, dsp_section_offset, dsp_section_size);                          //call it twice and the fs session is revoked, lol. on >= 9.0 this doesn't happen
		
		if(memcmp(dsp_hash_offset, sha256, 0x20)){ 
			fail=1;
			printf("sect %d hash failed %08X %08X\n", i, (int)dsp_hash_address, (int)dsp_section_size);
		}
		pos+=0x30;
		
	}
	
	return fail;
}

Result dumpCode(u64 tid , char* path)
{
	Result ret;
	Handle fileHandle;

	ret = openCode(&fileHandle, tid, 0);

	char name[50];

	sprintf(name, "%s.bin", path);

	u8* fileBuffer = NULL;
	u64 fileSize = 0;
	
	u32 bytesRead;

	ret = FSFILE_GetSize(fileHandle, &fileSize);
	if(ret)return ret;
	fileBuffer = (u8*)malloc(fileSize);
	if(ret)return ret;
	ret = FSFILE_Read(fileHandle, &bytesRead, 0x0, fileBuffer, fileSize);
	if(ret)return ret;
	ret = FSFILE_Close(fileHandle);
	if(ret)return ret;

	u32 decompressedSize = lzss_get_decompressed_size(fileBuffer, fileSize);
	u8* decompressedBuffer = (u8*)linearMemAlign(decompressedSize, 0x1000);
	if(!decompressedBuffer)return 1;

	lzss_decompress(fileBuffer, fileSize, decompressedBuffer, decompressedSize);
	free(fileBuffer);
	
	const char *magic = "DSP1";
	u8 *dsp_loc = NULL;
	u32 dsp_size = 0;
	
	dsp_loc = (u8*)memmem(decompressedBuffer , decompressedSize, magic, 4);
	if(dsp_loc){
		printf("Magic found! Beginning dsp dump! ...\n");
		dsp_size = *(u32*)(dsp_loc + 4);  //size usually 0xC25C
		dsp_loc -= 0x100;
		
		if(!checkHashes(dsp_loc)) printf("Sha256 verified! Size: %08X\nWriting dsp firm now ...\n", (int)dsp_size);
		else{
			printf("\x1b[31;1m");  //red text for scaring people
			printf("Sha256 mismatch! Size: %08X\n", (int)dsp_size);
			printf("Dsp firm will be dumped, but it may not work!\n");
			printf("\x1b[37;1m");  //back to white
			//linearFree(decompressedBuffer);
			//return 2;
		}
		
		FILE* f = fopen("sdmc:/3ds/dspfirm.cdc", "wb");
		if(!f) { linearFree(decompressedBuffer); return 2; }
		fwrite(dsp_loc, 1, dsp_size, f);
		fclose(f);
		
		printf("\nDsp firm written to: sdmc:/3ds/dspfirm.cdc\n\n");
		printf("No more action required!\n");
	}
	else{
		printf("Dsp magic not found!\n");
		linearFree(decompressedBuffer);
		return 1;
	}
	
	linearFree(decompressedBuffer);
	return 0;
}

void dumpDsp()
{
	int i = 0;
	Result res;

	for(i = 0; i < NB_TITLES; ++i){
		TitleInfo tl=titles[i];
		res = dumpCode(tl.titleid, tl.name);
		if(res) {
			printf("Not found: %s %08X\n", tl.name, (int)res);
		}
	}
}