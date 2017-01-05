#include <string>

#include "ndsheaderbanner.h"
#include <stdio.h>
#include <malloc.h>

const char* savedtext;

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

char* grabText(FILE* ndsFile) {
    sNDSHeader NDSHeader;
    sNDSBanner myBanner;
	
	fseek ( ndsFile , 0 , SEEK_SET );
    fread(&NDSHeader,1,sizeof(NDSHeader),ndsFile);
	
	if (NDSHeader.bannerOffset != 0x00000000) {
        fseek ( ndsFile , NDSHeader.bannerOffset , SEEK_SET );
        
        fread(&myBanner,1,sizeof(myBanner),ndsFile);
		
		int bnrtitlenum = 0;
		int size = sizeof(myBanner.titles[bnrtitlenum]);
		
		// turn unicode into ascii (kind of)
		int i;
		char *p = (char*)myBanner.titles[bnrtitlenum];
		for (i = 0; i < size; i = i+2) {
			p[i/2] = p[i];
		}
		
		savedtext = malloc(256);
		strncpy(savedtext, p, strlen(p)+1);

		return savedtext;
	} else {
		return "No Text";
	}
}

sf2d_texture* grabIcon(FILE* ndsFile) {
    sNDSHeader NDSHeader;
    sNDSBanner myBanner;
    
    u32 * textureData = (u32*) linearAlloc(1024*sizeof(u32));    
    
    fseek ( ndsFile , 0 , SEEK_SET );
    fread(&NDSHeader,1,sizeof(NDSHeader),ndsFile);
    
    if (NDSHeader.bannerOffset != 0x00000000) {
        fseek ( ndsFile , NDSHeader.bannerOffset , SEEK_SET );
        
        fread(&myBanner,1,sizeof(myBanner),ndsFile);    
        
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
    } else {
        return sfil_load_PNG_file("romfs:/graphics/icon_unknown.png", SF2D_PLACE_RAM); // use this if banner offset is 0
    }
}

// Duplicate of grabIcon, but stores icon in memory, needed for when the launching game moves up
sf2d_texture* grabandstoreIcon(FILE* ndsFile) {
    sNDSHeader NDSHeader;
    sNDSBanner myBanner;   
    
    fseek ( ndsFile , 0 , SEEK_SET );
    fread(&NDSHeader,1,sizeof(NDSHeader),ndsFile);
    
    if (NDSHeader.bannerOffset != 0x00000000) {
        fseek ( ndsFile , NDSHeader.bannerOffset , SEEK_SET );
        
        fread(&myBanner,1,sizeof(myBanner),ndsFile);    
        
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
    } else {
        return sfil_load_PNG_file("romfs:/graphics/icon_unknown.png", SF2D_PLACE_RAM); // use this if banner offset is 0
    }
}