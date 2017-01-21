// SMDH structs from FBI 2.4.7's util.h.
#ifndef TWLOADER_SMDH_H
#define TWLOADER_SMDH_H

typedef struct {
	u16 shortDescription[0x40];
	u16 longDescription[0x80];
	u16 publisher[0x40];
} SMDH_title;

typedef struct {
	char magic[0x04];
	u16 version;
	u16 reserved1;
	SMDH_title titles[0x10];
	u8 ratings[0x10];
	u32 region;
	u32 matchMakerId;
	u64 matchMakerBitId;
	u32 flags;
	u16 eulaVersion;
	u16 reserved;
	u32 optimalBannerFrame;
	u32 streetpassId;
	u64 reserved2;
	u8 smallIcon[0x480];
	u8 largeIcon[0x1200];
} SMDH;

#endif /* TWLOADER_SMDH_H */
