#ifndef GERMAN_H
#define GERMAN_H

#include <3ds/types.h>

// GERMAN
static const char *const lang_DE[STR_MAX] = {

	/** DS/DSi boot screen **/
	"WARNUNG - GESUNDHEIT UND SICHERHEIT",					// "WARNING - HEALTH AND SAFETY"
	"BEVOR DU ZU SPIELEN BEGINNST, LIES BITTE DIE",				// "BEFORE PLAYING, READ THE HEALTH"
	"GESUNDHEITS- UND SICHERHEITSHINWEISE",					// "AND SAFETY PRECAUTIONS BOOKLET"
	"BEZÜGLICH WICHTIGER INFORMATIONEN ZU DEINER",				// "FOR IMPORTANT INFORMATION"
	"GESUNDHEIT UND SICHERHEIT DURCH.",					// "ABOUT YOUR HEALTH AND SAFETY."
	"UM EINE EXTRA-KOPIE DES TEXTES FÜR DEINE REGION",			// "TO GET AN EXTRA COPY FOR YOUR REGION, GO ONLINE AT"
	"ZU BEKOMMEN, BESUCHE BITTE FOLGENDE WEBSEITE:",
	"www.nintendo.com/healthsafety/",					// "www.nintendo.com/healthsafety/"
	
	"Berühre den Touchscreen, um fortzufahren.",				// "Touch the Touch Screen to continue."
	
	/** GUI **/
	": Zurück zum HOME-Menü",						// "Return to HOME Menu"
	"START",								// "START" (used on cartridge indicator)
	"Keine Karte",								// "No cartridge"
	"Unbekannte Karte",							// "Unknown Cartridge"
	"Einstellungen",							// "Settings"
	": Zurück",								// "B: Back"	
	": Spiele hinzufügen",							// "Y: Add Games"

	/** Settings: GUI **/
	"Sprache",								// "Language"
	"Design",								// "Theme"
	"Farbe",								// "Color"
	"Menüfarbe",								// "Menu Color"
	"Dateiname anzeigen",							// "Show filename"
	"Spielzähler",								// "Game counter"
	"Eigener Hintergrund",							// "Custom bottom image"
	"TWLoader Update",							// "Auto-update TWLoader"
	"TWLoader Dateityp",							// "TWLoader filetype"
	"TWLoader Sofortupdate",						// "Update TWLoader"
	"DS/DSi Startanimation",						// "DS/DSi Boot Screen"
	"Gesundheit & Sicherheit",						// "Health and Safety message"
	"Startbildschirm",							// "Show Boot Screen"
	"ROM-Speicherort",							// "ROM path"
	"Schnellstart",								// "Quick start"

	/** Settings: NTR_mode **/
	"Flashkart(en) auswählen",						// "Flashcard(s) select"
	"Notification LED color",						// "Notification LED color"
	"ARM9 CPU Clock",							// "ARM9 CPU Speed"
	"Sound/Mikrofon-Frequenz",						// "Sound/Microphone frequency"
	"SD-Zugriff für Slot-1",						// "SD card access for Slot-1"
	"Slot-1 zurücksetzen",							// "Reset Slot-1"
	"Bootstrap Ladebildschirm",						// "Bootstrap loading screen"
	"Konsolenausgabe",							// "Console output"
	"Bootstrap autom. aktualisieren",					// "Auto-update bootstrap"
	"Bootstrap",								// "Bootstrap"

	/** Settings: Top Screen **/
	": Bootstrap aktualisieren (offiziell)",			// "X: Update bootstrap (Official release)"
	": Bootstrap aktualisieren (inoffiziell)",			// "Y: Update bootstrap (Unofficial release)"

	/** Settings: GUI values **/

	// Color
	"Grau",													// "Gray"
	"Braun",												// "Brown"
	"Rot",													// "Red"
	"Pink",												 	// "Pink"
	"Orange",												// "Orange"
	"Gelb",													// "Yellow"
	"Gelb-Grün",												// "Yellow-Green"
	"Grün 1",												// "Green 1"
	"Grün 2",												// "Green 2"
	"Hellgrün",												// "Light green"
	"Himmelblau",												// "Sky blue"
	"Hellblau",												// "Light blue"
	"Blau",													// "Blue"
	"Violett",												// "Violet"
	"Lila",													// "Purple"
	"Fuchsia",												// "Fuchsia"
	"Rot und blau",												// "Red and blue"
	"Grün und gelb",											// "Green and yellow"
	"Weihnachten",												// "Christmas"

	// Menu Color
	"Weiß",													// "White"
	"Grau-Schwarz",												// "Gray-Black"

	/** Settings: GUI descriptions **/
	"Die Sprache der Benutzeroberfläche",					// "The language to use for the UI,"
	"und der Anzeige von Spielnamen.",					// "including game banner text."

	"Das aktuelle Design für TWLoader.",					// "The theme to use in TWLoader."
	"Drücke START für Unterarten hiervon.",					// "Press START for sub-themes."

	"Die Farbe des oberen Hintergrunds,",					// "The color of the top background,"
	"Auswahlrahmens und der Startanimation.",				// "the START border, and the circling dots."

	"Die Farbe des oberen Rahmens",						// "The color of the top border,"
	"und des unteren Hintergrunds.",					// "and the bottom background."

	"Zeigt den Dateinamen des Spiels",					// "Shows game filename at the top of the bubble."
	"oben in der Blase an.",						// (empty)

	"Zeigt die Nummer des gewählten Spiels",				// "A number of selected game and listed games"
	"sowie die Gesamtzahl unter der Blase.",				// "is shown below the text bubble."

	"Lädt ein benutzerdefiniertes Bild für",				// "Loads a custom bottom screen image"
	"den unteren Bildschirm im Spiele-Menü.",				// "for the game menu."

	"Die neuste TWLoader Version beim Start",				// "Auto-download the CIA of the latest"
	"automatisch herunterladen.",						// "TWLoader version at launch."

	"Wann soll der DS/DSi Startbildschirm",					// "Where do you want to see the"
	"angezeigt werden?",							// "DS/DSi boot screen?"

	"Beim Start werden drahtlose Funktionen",				// "On startup, WiFi features and"
	"und der Banner-Download übersprungen.",				// "banner caching will be skipped."

	/** Settings: NTR/TWL_mode descriptions **/
	"Wähle die Flashkarte(n) aus, die du zum",				// "Pick a flashcard to use to"
	"Ausführen von ROMs benutzen willst.",					// "run ROMs from it."

	"Lege eine Farbe fest, in der die",					// "See rainbow colors glowing in"
	"Benachrichtigungs-LED leuchten soll.",					// "the Notification LED."

	"TWL auswählen, um Lags in",						// "Set to TWL to get rid of lags in some games."
	"einigen Spielen zu vermeiden.",					// (empty)

	"32.73 kHz: Originale Qualität",					// "32.73 kHz: Original quality"
	"47.61 kHz: Hohe Qualität",						// "47.61 kHz: High quality"

	// "Erlaubt 8 bit VRAM Schreibvorgänge",				// "Allows 8 bit VRAM writes"
	// "und erweitert den Bus auf 32 bit.",					// "and expands the bus to 32 bit."

	"Ermöglicht den Zugriff auf",						// "Enables SD card access for"
	"die SD-Karte für Slot-1 Karten.",					// "Slot-1 cards."

	"Wähle eine DS/DSi Startanimation zur",					// "Displays the DS/DSi boot animation"
	"Anzeige vor Spielstart oder ROM-Auswahl.",				// "before launched game."

	"Zeigt die Gesundheits- und Sicherheits-",				// "Displays the Health and Safety"
	"warnung auf dem unteren Bildschirm an.",				// "message on the bottom screen."

	"Aktivieren, wenn Slot-1 Karten mit",					// "Enable this if Slot-1 carts are stuck"
	"einem weißen Bildschirm hängen bleiben.",				// "on white screens."

	"Zeigt einen Ladebildschirm an, bevor die",				// "Shows a loading screen before ROM"
	"ROM in nds-bootstrap gestartet wird.",					// "is started in nds-bootstrap."

	"Zeigt Text zur Fehlerbehebung",					// "Displays some text before launched game."
	"vor dem Spielstart an.",						// (empty)

	// "Stellt den ARM9 SCFG_EXT fest,",					// "Locks the ARM9 SCFG_EXT,"
	// "vermeidet Konflikt mit aktuellem libnds",				// "avoiding conflict with recent libnds."

	"Aktualisiert nds-bootstrap bei jedem",					// "Auto-update nds-bootstrap at launch."
	"Start von TWLoader automatisch.",					// (empty)

	"Wechsle zwischen der offiziellen und",					// "Change between release and"
	"inoffiziellen nds-bootstrap Version.",					// "unofficial bootstrap file."

	/** Start menu **/
	// Options
	"Speicherort",										// "Game location"
	"Spielcover: Ein",									// "Box Art: On"
	"Spielcover: Aus",									// "Box Art: OFF"
	"ROM-Typ wählen",									// "Select ROM type"
	"Oberer Rahmen: Ein",									// "Top border: On"
	"Oberer Rahmen: Aus",									// "Top border: Off"	
	"Entferne Donor ROM",									// "Unset donor rom"	
	"Suche",										// "Search"
	// Values
	"SD-Karte",										// "SD Card"
	"Flashkarte",										// "Flashcard"
	"Benutze die Tastatur, um nach ROMs zu suchen",						// "Use the keyboard to find roms"

	/** Select ROM type **/
	"GBARunner2 starten",									// "Start GBARunner2"

	/** Select menu **/
	// Options
	"ARM9 CPU Clock",									// "ARM9 CPU Speed"
	"SD-Zugriff",										// "SD access"
	"Donor ROM benutzen",									// "Use set donor ROM"
	"Setze als Donor ROM",									// "Set as donor ROM"
	"LED-Farbe",										// "Set LED color"
	// Values
	"Standard",										// "Default"

	/** Sub-theme **/
	"Unterauswahl: DSi Menu",								// "Sub-theme select: DSi Menu"
	"Unterauswahl: R4",									// "Sub-theme select: R4"
	"Unterauswahl: akMenu/Wood",								// "Sub-theme select: akMenu/Wood"

	"Für dieses Design existiert keine Unterauswahl.",					// "No sub-themes exist for this theme."

	/** Settings others minor strings **/
	"/: Speichern und zurück",								// "A/B: Save and Return"
	"Links/Rechts: Auswählen",								// "Left/Right: Pick"
	"Einstellungen: GUI",									// "Settings: GUI"
	"Einstellungen: NTR Modus",								// "Settings: NTR mode"

};


#endif /* GERMAN_H */
