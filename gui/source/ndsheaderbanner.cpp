#include <string>

#include "ndsheaderbanner.h"
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>

#include "log.h"

u32 * storedtextureData = (u32*) linearAlloc(1024*sizeof(u32));  

u32 colorConvert(u32 iconPixel, u16 indice) {
    u8 a = 255;
    if(!indice) {
        a = 0;
    }
    u8 r = ((iconPixel & 0x1F ) * 255) / 31;
    u8 g = (((iconPixel >> 5) & 0x1F) * 255) / 31;
    u8 b = (((iconPixel >> 10) & 0x1F) * 255) / 31;
    
    return RGBA8(r, g, b, a) ;
}

/**
 * Get the title ID.
 * @param ndsFile DS ROM image.
 * @param buf Output buffer for title ID. (Must be at least 4 characters.
 */
void grabTID(FILE* ndsFile, char *buf) {
	fseek(ndsFile, offsetof(sNDSHeadertitlecodeonly, gameCode), SEEK_SET);
	fread(buf, 1, 4, ndsFile);
}

char* grabText(FILE* binFile, int bnrtitlenum, int line) {
    sNDSBanner myBanner;
	
	if (bnrtitlenum == 8 || bnrtitlenum == 9 || bnrtitlenum == 10)	// If language is Dutch, Portugese, or Russian
		bnrtitlenum = 5;	// set to Spanish
	else if (bnrtitlenum == 12)		// If language is Traditional Chinese
		bnrtitlenum = 6;	// set to (Simplified) Chinese
	
	fseek ( binFile , 0 , SEEK_SET );
        
	fread(&myBanner,1,sizeof(myBanner),binFile);
	
	if (myBanner.version != 0x0000) {
        if (myBanner.version == 0x0002) {
			if (bnrtitlenum == 7)
				bnrtitlenum = 0;	// Use Japanese language if title has no Korean text
		} else if (myBanner.version == 0x0001) {
			if (bnrtitlenum == 6 || bnrtitlenum == 7)
				bnrtitlenum = 0;	// Use Japanese language if title has no Chinese and Korean text
		}

		int size = sizeof(myBanner.titles[bnrtitlenum]);
		
		// turn unicode into ascii (kind of)
		int i;
		int i2;
		int i3;
		char *p = (char*)myBanner.titles[bnrtitlenum];
		char *p2 = (char*)myBanner.titles[bnrtitlenum]+2;
		char *p3 = (char*)myBanner.titles[bnrtitlenum]+4;
		
		for (i = 0; i < size; i = i+2) {
			/* if ((p[i] == 0x0A) || (p[i] == 0xFF)) {
				if (line == 0)
					p[i/2] = 0;
				else if (line >= 1) {
					for (i2 = 0; i2 < size; i2 = i2+2) {
						if ((p2[i2+i] == 0x0A) || (p2[i2+i] == 0xFF)) {
							if (line == 1)
								p2[i2/2] = 0;
							else if (line == 2) {
								for (i3 = 0; i3 < size; i3 = i3+2) {
									if ((p3[i3+i] == 0x0A) || (p3[i3+i] == 0xFF))
										p3[i3/2] = 0;									
									else
										p3[i3/2] = p3[i3+i];	// write to line 2
								}
							}
						} else {
							if (line == 1)
								p2[i2/2] = p2[i2+i];	// write to line 1
						}
					}
				}
			} else {
				if (line == 0) */
					p[i/2] = p[i];	// write to line 0
			// }
		}
		
		char *savedtext = (char*)malloc(256);
		if (line == 0) {
			strncpy(savedtext, p, strlen(p)+1);
		} else if (line == 1) {
			strncpy(savedtext, p2, strlen(p2)+1);
		} else if (line == 2) {
			strncpy(savedtext, p3, strlen(p3)+1);
		}
		return savedtext;
	} else {
		if (line == 0)
			return "No Info";
		else if (line == 1)
			return " ";
		else if (line == 2)
			return " ";
	}
}

void cacheBanner(FILE* ndsFile, const char* filename, sftd_font* setfont) {
	char path[256];
	snprintf(path, sizeof(path), "sdmc:/_nds/twloader/bnricons/%s.bin", filename);
	
	if (access(path, F_OK ) == -1) {
		FILE* filetosave = fopen(path, "wb");

		sNDSHeader NDSHeader;
		sNDSBannersize1 myBannersize1;
		sNDSBannersize2 myBannersize2;
		sNDSBannersize3 myBanner;
		
		LogFMA("NDSBannerHeader.cacheBanner", "Reading .NDS file:", filename);
		fseek ( ndsFile , 0 , SEEK_SET );
		fread(&NDSHeader,1,sizeof(NDSHeader),ndsFile);
		LogFMA("NDSBannerHeader.cacheBanner", ".NDS file read:", filename);
		
		if (NDSHeader.bannerOffset != 0x00000000) {
			fseek ( ndsFile , NDSHeader.bannerOffset , SEEK_SET );
			
			fread(&myBanner,1,sizeof(myBanner),ndsFile);
			
			sftd_draw_textf(setfont, 12, 32, RGBA8(0, 0, 0, 255), 12, "Now caching banner data (SD Card)...");
			sf2d_end_frame();
			sf2d_swapbuffers();
			LogFMA("NDSBannerHeader.cacheBanner", "Caching banner data: ", path);
			if (myBanner.version == 0x0103 || myBanner.version == 0x0003) {
				fwrite(&myBanner,1,sizeof(myBanner),filetosave);
			} else if (myBanner.version == 0x0002) {
				fwrite(&myBanner,1,sizeof(myBannersize2),filetosave);
			} else {
				fwrite(&myBanner,1,sizeof(myBannersize1),filetosave);
			}

			LogFMA("NDSBannerHeader.cacheBanner", "Banner data cached: ", path);
		} else {
			FILE* nobannerFile = fopen("romfs:/notextbanner", "rb");
			
			sftd_draw_textf(setfont, 12, 32, RGBA8(0, 0, 0, 255), 12, "Now caching banner data (SD Card)...");
			sf2d_end_frame();
			sf2d_swapbuffers();
			LogFMA("NDSBannerHeader.cacheBanner", "Caching banner data (empty): ", path);
			fread(&myBanner,1,sizeof(myBanner),nobannerFile);
			fwrite(&myBanner,1,sizeof(myBanner),filetosave);
			LogFMA("NDSBannerHeader.cacheBanner", "Banner data cached (empty): ", path);
			fclose(nobannerFile);
		}
		fclose(filetosave);
	}else{
		sftd_draw_textf(setfont, 12, 32, RGBA8(0, 0, 0, 255), 12, "Banner data already exists.");
		sf2d_end_frame();
		sf2d_swapbuffers();
	}
}

sf2d_texture* grabIcon(FILE* binFile) {
    sNDSBanner myBanner;
    
    u32 * textureData = (u32*) linearAlloc(1024*sizeof(u32));    
    
    fseek ( binFile , 0 , SEEK_SET );
        
    fread(&myBanner,1,sizeof(myBanner),binFile);    
        
    //u8 icon[512];            //!< 32*32 icon of the game with 4 bit per pixel paletted
    //u16 palette[16];      // color 0 is transparent, ABGR
        
    int offset = 0;
    
    for(int y = 0; y < 32; y += 8)
        for(int x = 0; x < 32; x += 8)
            for(int y2 = 0; y2 < 8; y2++)
                for(int x2 = 0; x2 < 8; x2+=2)
                {
                    u8 twopixel = myBanner.icon[offset];
                    
                    u32 firstPixel = twopixel & 0x0F;
                    u32 secondPixel = (twopixel & 0xF0) >> 4; 
                    
                    textureData[x + x2 + (y + y2)*64] = colorConvert(myBanner.palette[firstPixel], firstPixel);
                    
                    textureData[x + x2 + 1 + (y + y2)*64] =  colorConvert(myBanner.palette[secondPixel], secondPixel);

                    offset++;
                }
	
    return sf2d_create_texture_mem_RGBA8(textureData, 64, 32, TEXFMT_RGBA8, SF2D_PLACE_RAM);
}

// Duplicate of grabIcon, but stores icon in memory, needed for when the launching game moves up
sf2d_texture* grabandstoreIcon(FILE* binFile) {
    sNDSBanner myBanner;   
    
    fseek ( binFile , 0 , SEEK_SET );
        
    fread(&myBanner,1,sizeof(myBanner),binFile);    
        
    //u8 icon[512];            //!< 32*32 icon of the game with 4 bit per pixel paletted
    //u16 palette[16];      // color 0 is transparent, ABGR
        
    int offset = 0;
    
    for(int y = 0; y < 32; y += 8)
        for(int x = 0; x < 32; x += 8)
            for(int y2 = 0; y2 < 8; y2++)
                for(int x2 = 0; x2 < 8; x2+=2)
                {
                    u8 twopixel = myBanner.icon[offset];
                    
                    u32 firstPixel = twopixel & 0x0F;
                    u32 secondPixel = (twopixel & 0xF0) >> 4; 
                    
                    storedtextureData[x + x2 + (y + y2)*64] = colorConvert(myBanner.palette[firstPixel], firstPixel);
                    
                    storedtextureData[x + x2 + 1 + (y + y2)*64] =  colorConvert(myBanner.palette[secondPixel], secondPixel);

                    offset++;
                }
	
    return sf2d_create_texture_mem_RGBA8(storedtextureData, 64, 32, TEXFMT_RGBA8, SF2D_PLACE_RAM);
}
