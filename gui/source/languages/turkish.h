#ifndef TURKISH_H
#define TURKISH_H

#include <3ds/types.h>

// TURKISH
static const char *const lang_TR[STR_MAX] = {
	
	/** DS/DSi boot screen **/
	"UYARI - SAĞLIK VE GÜVENLİK",								// "WARNING - HEALTH AND SAFETY"
	"OYNAMADAN ÖNCE, SAĞLIK VE",							// "BEFORE PLAYING, READ THE HEALTH"
	"GÜVENLİK ÖNLEMLERİ KİTAPÇIĞINI OKUYUN",							// "AND SAFETY PRECAUTIONS BOOKLET"
	"SAĞLIK VE GÜVENLİĞİNİZ HAKKINDA",								// "FOR IMPORTANT INFORMATION"
	"ÖNEMLİ BİLGİLER İÇİN.",							// "ABOUT YOUR HEALTH AND SAFETY."
	"BÖLGENİZE GÖRE EKSTRA BİR KİTAPÇIK KOPYASI",            // "TO GET AN EXTRA COPY FOR YOUR REGION, GO ONLINE AT"
	"ALMAK İÇİN ZİYARET EDİN",		
	"www.nintendo.com/healthsafety/",							// "www.nintendo.com/healthandsafety/"
	
	"Devam etmek için dokunmatik ekrana dokunun.",						// "Touch the Touch Screen to continue."
	
	/** GUI **/
	": Ana ekrana dön",									// "Return to HOME Menu"
	"BAŞLAT",													// "START" (used on cartridge indicator)
	"Kartuş yok",												// "No cartridge"
	"Tanımlanamayan kartuş", 										// "Unknown Cartridge"
	"Ayarlar",													// "Settings"
	": Geri",													// "B: Back"	
	": Oyun Ekle",												// "Y: Add Games"

	/** Settings: GUI **/
	"Dil",													// "Language"
	"Tema",													// "Theme"
	"Renk",													// "Color"
	"Menü Rengi",												// "Menu Color"
	"Dosya adını göster",											// "Show filename"
	"Oyun  sayacı",												// "Game counter"
	"Özel alt ekran görseli",										// "Custom bottom image"
	"TWLoader'ı otomatik güncelleştir",										// "Auto-update TWLoader"
	"TWLoader dosya tipi",										// "TWLoader filetype"
	"TWLoader'ı Güncelleştir",											// "Update TWLoader"
	"DS/DSi Başlangıç Ekranı",										// "DS/DSi Boot Screen"
	"Sağlık ve Güvenlik mesajı",								// "Health and Safety message"
	"Başlangıç Ekranını Göster",											// "Show Boot Screen"
	"ROM dizini",													// "ROM path"

	/** Settings: NTR/TWL_mode **/
	"Flashkart(ları) seç",										// "Flashcard(s) select"
	"Gökkuşağı LED",												// "Rainbow LED"
	"ARM9 CPU Hızı",											// "ARM9 CPU Speed"
	"Slot-1 için SD card erişimi",								// "SD card access for Slot-1"
	"Slot-1'i sıfırla",												// "Reset Slot-1"
	"Konsol çıktısı",											// "Console output"
	"Bootstrap'ı otomatik güncelleştir",									// "Auto-update bootstrap"
	"Bootstrap",												// "Bootstrap"

	/** Settings: Top Screen **/
	": Bootstrap'ı güncelleştir (Resmi sürüm)",					// "X: Update bootstrap (Official release)"
	": Bootstrap'ı güncelleştir (Resmi olmayan sürüm)",					// "Y: Update bootstrap (Unofficial release)"

	/** Settings: GUI values **/

	// Color
	"Gri",														// "Gray"
	"Kahverengi",													// "Brown"
	"Kırmızı",														// "Red"
	"Pembe",												 		// "Pink"
	"Turuncu",													// "Orange"
	"Sarı",													// "Yellow"
	"Sarı-Yeşil",												// "Yellow-Green"
	"Yeşil 1",													// "Green 1"
	"Yeşil 2",													// "Green 2"
	"Açık yeşil",												// "Light green"
	"Gök mavisi",													// "Sky blue"
	"Açık mavi",												// "Light blue"
	"Mavi",														// "Blue"
	"Menekşe",													// "Violet"
	"Mor",													// "Purple"
	"Fuşya",													// "Fuchsia"
	"Kırmızı&mavi",													// "Red and blue"
	"Yeşil&sarı",												// "Green and yellow"
	"Noel",												// "Christmas"

	// Menu Color
	"Beyaz",													// "White"
	"Siyah",													// "Black"

	/** Settings: GUI descriptions **/
	"Arayüzde kullanılacak dil,",							// "The language to use for the UI,"
	"Bu dil oyun açıklamasında da kullanılır.",								// "including game banner text."

	"TWLoader'da kullanılacak tema.",							// "The theme to use in TWLoader."
	"Yan temalar için BAŞLAT tuşuna basın.",								// "Press START for sub-themes."

	"Üst ekran arkaplan rengi,",							// "The color of the top background,"
	"Başlat kenarlığı ve daire çizgileri",					// "the START border, and the circling dots."

	"Üst kenarlık rengi,",								// "The color of the top border,"
	"ve alt ekran arkaplanı.",								// "and the bottom background."

	"Oyunun adını baloncuğun",								// "Shows game filename at the"
	"üst tarafında gösterir.",										// "top of the bubble."

	"Seçilen oyun ve sayısı",						// "A number of selected game and listed"
	"metin baloncuğunun altında görünür.",					// "games is shown below the text bubble."

	"DSi Menü teması için",						// "Loads a custom bottom screen image"
	"özel bir alt ekran görseli yükler.",									// "for the DSi Menu theme."

	"Açılışta son TWLoader versiyonuna",								// "Auto-update to the latest"
	"otomatik olarak güncelle",								// "TWLoader version at launch."

	"DS/DSi başlangıç ekranını",								// "Where do you want to see the"
	"Nerede görmek istersiniz?",										// "DS/DSi boot screen?"

	/** Settings: NTR/TWL_mode descriptions **/
	"ROMları yükleyeceğiniz bir",								// "Pick a flashcard to use to"
	"flashkart seçin.",										// "run ROMs from it."

	"Gökkuşağı renklerinin bildirim",							// "See rainbow colors glowing in"
	"LED'inde parladığını görün.",									// "the Notification LED."

	"Bazı oyunlarda TWL'nin takılmaları",							// "Set to TWL to get rid of lags"
	"atlatabilmesi için seçin.",											// "in some games."

	// "8 bit VRAM yazmalarına izin verir",									// "Allows 8 bit VRAM writes"
	// "ve veri yolunu 32 bite çıkarır.",							// "and expands the bus to 32 bit."

	"Slot-1 kartlar için",								// "Enables SD card access for"
	"SD kart erişimini etkinleştirir.",											// "Slot-1 cards."

	"Oyun başlamadan önce",						// "Displays the DS/DSi boot animation"
	"DS/DSi başlangıç ekranını gösterir.",									// "before launched game."

	"Alt ekranda Sağlık ve Güvenlik",							// "Displays the Health and Safety"
	"mesajını gösterir.",							// "message on the bottom screen."

	"Eğer Slot-1 kartlar beyaz ekranda",					// "Enable this if Slot-1 carts are stuck"
	"takılı kalıyorsa bunu etkinleştirin.",										// "on white screens."

	"Oyun başlamadan önce bazı yazılar gösterir.",					// "Displays some text before launched game."
	"",															// (empty)

	// "ARM9 SCFG_EXT'ı kilitler,",									// "Locks the ARM9 SCFG_EXT,"
	// "son libnds ile çakışmayı önler.",					// "avoiding conflict with recent libnds."

	"Açılışta nds-bootstrap'i otomatik güncelleştir.",						// "Auto-update nds-bootstrap at launch."
	"",															// (empty)

	"Bootstrap sürümü seçin,",								// "Pick release, unofficial,"
	"Resmi ya da resmi olmayan bootstrap sürümü.",										// "or old bootstrap."

	/** Start menu **/
	// Options
	"Oyun konumu",											// "Game location"
	"Kapak Resmi: Açık",												// "Box Art: On"
	"Kapak Resmi: Kapalı",												// "Box Art: OFF"
	"GBARunner2'yi Başlat",											// "Start GBARunner2"
	"Üst Kenarlık: Açık",											// "Top border: On"
	"Üst Kenarlık: Kapalı",											// "Top border: Off"
	"Donor Rom seçimini kaldır",											// "Unset donor rom"
	"Ara",													// "Search"
	// Values
	"SD Kart",													// "SD Card"
	"Flashkart",												// "Flashcard"
	"Romları bulmak için klavyeyi kullan",							// "Use the keyboard to find roms"

	/** Select menu **/
	// Options
	"ARM9 CPU Hızı",											// "ARM9 CPU Speed"
	"SD erişimi",												// "SD access"
	"Donor ROM seçmeyi kullan",										// "Use set donor ROM"
	"Donor ROM olarak ayarla",											// "Set as donor ROM"
	"LED rengini ayarla",											// "Set LED color"
	// Values
	"Varsayılan",													// "Default"

	/** Sub-theme **/
	"Yan tema seç: DSi Menüsü",								// "Sub-theme select: DSi Menu"
	"Yan tema seç: R4",										// "Sub-theme select: R4"
	"Yan tema seç: akMenu/Wood",							// "Sub-theme select: Wood"

	"Bu tema için yan tema mevcut değil.",						// "No sub-themes exist for this theme."

	/** Settings others minor strings **/
	"/: Kaydet ve Çık",										// "A/B: Save and Return"
	"Left/Right: Seç",											// "Left/Right: Pick"
	"Settings: Arayüz",											// "Settings: GUI"
	"Settings: NTR/TWL-modu",									// "Settings: NTR/TWL-mode"

};


#endif /* TURKISH_H */
