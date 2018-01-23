#ifndef FINNISH_H
#define FINNISH_H

#include <3ds/types.h>

// FINNISH
static const char *const lang_FI[STR_MAX] = {
	
	/** DS/DSi boot screen **/
	"VAROITUS - HYVINVOINTI JA TURVALLISUUS",					// "WARNING - HEALTH AND SAFETY"
	"ENNEN PELAAMISTA, LUE  HYVINVOINTI",						// "BEFORE PLAYING, READ THE HEALTH"
	"JA TURVALLISUUS VAROTOIMI VIHKO",						// "AND SAFETY PRECAUTIONS BOOKLET"
	"HYVINVOINTIIN JA TERVEYTEEN",							// "FOR IMPORTANT INFORMATION"
	"LIITTYVIEN ASIOIDEN KANNALTA",							// "ABOUT YOUR HEALTH AND SAFETY."
	"SAADAKSESI EKSTRA KOPION OMAN ALUEEN VIHKOSTA,",				// "TO GET AN EXTRA COPY FOR YOUR REGION, GO ONLINE AT"
	"MENE NETTISIVULLE",
	"www.nintendo.com/healthsafety/",						// "www.nintendo.com/healthsafety/"
	
	"Kosketa ruutua jatkaaksesi.",			        			// "Touch the Touch Screen to continue."
	
	/** GUI **/
	": Palaa HOME Valikkoon",							// "Return to HOME Menu"
	"KÄYNNISTÄ",									// "START" (used on cartridge indicator)
	"Ei pelikorttia",								// "No cartridge"
	"Tuntemton pelikortti", 							// "Unknown Cartridge"
	"Asetukset",									// "Settings"
	": Takaisin",									// "B: Back"	
	": Lisää pelejä",								// "Y: Add Games"

	/** Settings: GUI **/
	"Kieli",									// "Language"
	"Teema",									// "Theme"
	"Väri",										// "Color"
	"Valikon väri",									// "Menu Color"
	"Näytä tiedostonimi",		     						// "Show filename"
	"Peli laskuri",									// "Game counter"
	"Mukautettu alakuva",								// "Custom bottom image"
	"Auto-päivitä TWLoader",							// "Auto-update TWLoader"
	"TWLoader tiedostotyyppi",							// "TWLoader filetype"
	"Päivitä TWLoader",								// "Update TWLoader"
	"DS/DSi Alku Ruutu",								// "DS/DSi Boot Screen"
	"H&S Viesti",									// "Health and Safety message"
	"Näytä alku ruutu",								// "Show Boot Screen"
	"ROM polku",									// "ROM path"
	"Quick start",												// "Quick start"

	/** Settings: NTR_mode **/
	"Flashkortin valinta",								// "Flashcard(s) select"
	"Notification LED color",									// "Notification LED color"
	"ARM9 CPU nopeus",								// "ARM9 CPU Speed"
	"Ääni/Mikrofooni taajuus",						    	// "Sound/Microphone frequency"
	"SD-kortti lupa Slot-1:delle",							// "SD card access for Slot-1"
	"Nollaa Slot-1",								// "Reset Slot-1"
	"Bootstrap lataus ruutu",							// "Bootstrap loading screen"
	"Konsolin ulostulo",								// "Console output"
	"Auto-päivitä bootstrap",							// "Auto-update bootstrap"
	"Bootstrap",									// "Bootstrap"

	/** Settings: Top Screen **/
	": Päivitä bootstrap (Julkaisu)",						// "X: Update bootstrap (Official release)"
	": Päivitä bootstrap (Epävirallinen)",						// "Y: Update bootstrap (Unofficial release)"

	/** Settings: GUI values **/

	// Color
	"Harmaa",									// "Gray"
	"Ruskea",									// "Brown"
	"Punainen",									// "Red"
	"Vaalean punainen",								// "Pink"
	"Oranssi",									// "Orange"
	"Keltainen",									// "Yellow"
	"Kelta-vihreä",									// "Yellow-Green"
	"Vihreä 1",									// "Green 1"
	"Vihreä 2",									// "Green 2"
	"Vaalean vihreä",								// "Light green"
	"Taivaan sininen",								// "Sky blue"
	"Vaalean sininen",								// "Light blue"
	"Sininen",									// "Blue"
	"Violetti",									// "Violet"
	"Purppura",									// "Purple"
	"Fuksia",									// "Fuchsia"
	"Punainen&sininen",								// "Red and blue"
	"Vihreä&keltainen",								// "Green and yellow"
	"Joulu",									// "Christmas"

	// Menu Color
	"Valkoinen",									// "White"
	"Harmaa-Musta",									// "Gray-Black"

	/** Settings: GUI descriptions **/
	"Kieli jota käytetään käyttöliittymässä,",					// "The language to use for the UI,"
	"mukaan lukien pelien kuvien teksissä.",					// "including game banner text."

	"Teema jota käytetään TWLoaderissa. Paina",					// "The theme to use in TWLoader."
	"START päästäksesi ali-teemavalikkoon.",					// "Press START for sub-themes."

	"Ylätaustan väri,",								// "The color of the top background,"
	"START reunus, ja pyörivät pisteet.",						// "the START border, and the circling dots."

	"Yläreunan,",									// "The color of the top border,"
	"ja alataustan väri.",								// "and the bottom background."

	"Näyttää tiedostonimen",							// "Shows game filename at the"
	"kuplassa.",									// "top of the bubble."

	"Näyttää valitun ja listattujen",						// "A number of selected game and listed"
	"pelien numeron kuplassa.",							// "games is shown below the text bubble."

	"Lataa mukautetun kuvan alaruudulle",						// "Loads a custom bottom screen image"
	"DSi Valikko taustalla.",							// "for the DSi Menu theme."

	"Auto-päivitä viimeisimpään",							// "Auto-update to the latest"
	"TWLoader versioon käynnistyessä.",						// "TWLoader version at launch."

	"Missä haluat käyttää",								// "Where do you want to see the"
	"DS/DSi alku kuvaa?",								// "DS/DSi boot screen?"

	"On startup, WiFi features and",							// "On startup, WiFi features and"
	"banner caching will be skipped.",							// "banner caching will be skipped."

	/** Settings: NTR/TWL_mode descriptions **/
	"Valitse flash-kortti jolta voi",						// "Pick a flashcard to use to"
	"pelata pelejä.",								// "run ROMs from it."

	"Nää sateenkaaren väreja",							// "See rainbow colors glowing in"
	"ilmoitus LEDissä.",								// "the Notification LED."

	"Aseta TWL päästäksesi eroon lagista",						// "Set to TWL to get rid of lags"
	"joissain peleissä.",								// "in some games."

	"32.73 kHz: Alkuperäinen laatu",						// "32.73 kHz: Original quality"
	"47.61 kHz: Korkea laatu",							// "47.61 kHz: High quality"

	// "Antaa tehdä 8-bittisiä VRAM kirjoituksia",					// "Allows 8 bit VRAM writes"
	// "ja suurentaa bussia 32-bittiseksi.",					// "and expands the bus to 32 bit."

	"Antaa SD-kortti luvan",							// "Enables SD card access for"
	"Slot-1 pelikorteille.",							// "Slot-1 cards."

	"Näyttää DS/DSi alku animaation",						// "Displays the DS/DSi boot animation"
	"ennen pelin alkua.",								// "before launched game."

	"Näyttää Hyvinvointi ja terveys",						// "Displays the Health and Safety"
	"viestin alaruudulla.",								// "message on the bottom screen."

	"Ota tämä käyttöön jos Slot-1 pelikortit",					// "Enable this if Slot-1 carts are stuck"
	"ovat jumissa valkoisella ruudulla.",						// "on white screens."

	"Näyttää latausruudun ennen kuin ROM",						// "Shows a loading screen before ROM"
	"käynnistyy nds-bootstrapissa.",						// "is started in nds-bootstrap."

	"Näyttää vähän tekstiä ennen pelin alkua.",					// "Displays some text before launched game."
	"",																	// (empty)

	// "Lukitsee ARM9 SCFG_EXT:in,",						// "Locks the ARM9 SCFG_EXT,"
	// "välttäen konfliktin uudempien libnds:sien kanssa.",				// "avoiding conflict with recent libnds."

	"Auto-päivitä nds-bootstrap käynnistyessä.",					// "Auto-update nds-bootstrap at launch."
	"",																	// (empty)

	"Valitse julkaistu tai epävirallinen",				// "Pick release or unofficial bootstrap."
	"bootstrap.",										// (empty)

	/** Start menu **/
	// Options
	"Pelien sijainti",								// "Game location"
	"Boxi kuvat: On",								// "Box Art: On"
	"Boxi kuvat: Off",								// "Box Art: OFF"
	"Suorita GBARunner2",								// "Start GBARunner2"
	"Yläreunus: On",								// "Top border: On"
	"Yläreunus: Off",								// "Top border: Off"
	"Lopeta ROMin lahjoitus",							// "Unset donor rom"
	"Etsi",										// "Search"
	// Values
	"SD-kortti",									// "SD Card"
	"Flashkortti",									// "Flashcard"
	"Käytä näppäimistöä ROMien löytämiseen",					// "Use the keyboard to find roms"

	/** Select menu **/
	// Options
	"ARM9 CPU nopeus",								// "ARM9 CPU Speed"
	"SD-kortti lupa",								// "SD access"
	"Käytä luovuttaja ROMia",							// "Use set donor ROM"
	"Tee luovuttaja ROMiksi",							// "Set as donor ROM"
	"Valitse LEDin väri",								// "Set LED color"
	// Values
	"Oletusarvo",									// "Default"

	/** Sub-theme **/
	"Ali-teeman valinta: DSi Valikko",						// "Sub-theme select: DSi Menu"
	"Ali-teeman valinta: R4",							// "Sub-theme select: R4"
	"Ali-teeman valinta: akMenu/Wood",						// "Sub-theme select: Wood"

	"Tällä teemalla ei ole ali-teemoja.",						// "No sub-themes exist for this theme."

	/** Settings others minor strings **/
	"/: Tallenna ja poistu",							// "A/B: Save and Return"
	"Vasen/Oikea: Valitse",								// "Left/Right: Pick"
	"Asetukset: Käyttöliittymä",							// "Settings: GUI"
	"Asetukset: NTR-tila",								// "Settings: NTR mode"

};


#endif /* FINNISH_H */
