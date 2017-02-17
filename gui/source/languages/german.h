#ifndef GERMAN_H
#define GERMAN_H

#include <3ds/types.h>

// GERMAN
static const char *const lang_DE[STR_MAX] = {
	
	/** GUI **/
    ": Zurück zum HOME-Menü",									// "Return to HOME Menu"
	"START",													// "START" (used on cartridge indicator)
	"Keine Karte",												// "No cartridge"
	"Unbekannte Karte",											// "Unknown Cartridge"
	"Einstellungen",											// "Settings"
	"B: Zurück",												// "B: Back"	
	
	/** Settings: GUI **/
	"Sprache",													// "Language"
	"Design",													// "Theme"
	"Farbe",													// "Color"
	"Menüfarbe",												// "Menu Color"
	"Dateiname anzeigen",										// "Show filename"
	"Spielzähler",												// "Game counter"
	"Benutzerdefiniertes unteres Bild",							// "Custom bottom image"
	"Bootstrap automatisch updaten",							// "Auto-update bootstrap"
	"TWLoader automatisch updaten",								// "Auto-update to latest TWLoader"
	
	/** Settings: NTR/TWL_mode **/
	"Flashkart(en) auswählen",									// "Flashcard(s) select"
	"Regenbogen LED",											// "Rainbow LED"
	"ARM9 CPU Geschwindigkeit",									// "ARM9 CPU Speed"
	"VRAM Beschleunigung",										// "VRAM boost"
	"DS/DSi Startbildschirm",									// "DS/DSi Boot Screen"
	"Gesundheits- und Sicherheitsinformationen",				// "Health and Safety message"
	"Slot-1 zurücksetzen",										// "Reset Slot-1"
	"Konsolenausgabe",											// "Console output"
	"ARM9 SCFG_EXT feststellen",								// "Lock ARM9 SCFG_EXT"
	"Bootstrap",												// "Bootstrap"
	
	/** Settings: Top Screen **/
	"X: Bootstrap updaten (Offizielle Version)",				// "X: Update bootstrap (Official release)"
	"Y: Bootstrap updaten (Inoffizielle Version)",				// "Y: Update bootstrap (Unofficial release)"
    "START: TWLoader aktualisieren",							// "START: Update TWLoader"
	
	/** Settings: GUI values **/
	
	// Color
	"Grau",														// "Gray"
	"Braun",													// "Brown"
	"Rot",														// "Red"
	"Pink",												 		// "Pink"
	"Orange",													// "Orange"
	"Gelb",														// "Yellow"
	"Gelb-Grün",												// "Yellow-Green"
	"Grün 1",													// "Green 1"
	"Grün 2",													// "Green 2"
	"Hellgrün",													// "Light green"
	"Himmelblau",												// "Sky blue"
	"Hellblau",													// "Light blue"
	"Blau",														// "Blue"
	"Violett",													// "Violet"
	"Lila",														// "Purple"
	"Fuchsia",													// "Fuchsia"
	"Rot und blau",												// "Red and blue"
	"Grün und gelb",											// "Green and yellow"
	"Weihnachten",												// "Christmas"

	// Menu Color
	"Weiß",														// "White"
	"Schwarz",													// "Black"

	/** Settings: GUI descriptions **/
	"Die Sprache der Benutzeroberfläche",						// "The language to use for the UI,"
	"und der Spielenamen.",										// "including game banner text."
	
	"Das benutzte Design für TWLoader.",						// "The theme to use in TWLoader."
	"Drücke A für Unterdesigns.",								// "Press A for sub-themes."
	
	"Die Farbe des oberen Hintergrunds,",						// "The color of the top background,"
	"der START Umrandung und der kreisenden Punkte.",			// "the START border, and the circling dots."
	
	"Die Farbe des oberen Randes",								// "The color of the top border,"
	"und des unteren Hintergrundes.",							// "and the bottom background."
	
	"Zeigt den Dateinamen des Spiels oben in der Blase.",		// "Shows game filename at the top of the bubble."
	"",															// (empty)

	"Die Nummer des ausgewählten Spiels und die",				// "A number of selected game and listed games"
	"Anzahl der Spiele wird unter der Textblase angezeigt.",	// "is shown below the text bubble."
	
	"Läd ein benutzerdefiniertes Bild für den unteren",			// "Loads a custom bottom screen image"
	"Bildschirm im Spiele-Menü.",								// "for the game menu."
	
	"nds-bootstrap beim Start automatisch updaten.",			// "Auto-update nds-bootstrap at launch."
	"",															// (empty)

	"Die neuste TWLoader Version beim Start",					// "Auto-download the CIA of the latest"
	"automatisch herunterladen.",								// "TWLoader version at launch."
	
	/** Settings: NTR/TWL_mode descriptions **/
	"Wähle die Flashkarte, die du zum",							// "Pick a flashcard to use to"
	"ausführen von ROMs benutzen willst.",						// "run ROMs from it."
	
	"Aktiviere Regenbogen Farben für",							// "See rainbow colors glowing in"
	"die Benachrichtigungs-LED.",								// "the Notification LED."
	
	"Stelle auf TWL, um Lags in manchen",						// "Set to TWL to get rid of lags in some games."
	"Spielen zu vermeiden.",									// (empty)

	"Erlaubt 8 bit VRAM Schreibevorgänge",						// "Allows 8 bit VRAM writes"
	"und erweitert den Bus auf 32 bit.",						// "and expands the bus to 32 bit."
	
	"Zeigt die DS/DSi Boot-Animation",							// "Displays the DS/DSi boot animation"
	"vor dem Spielstart an.",									// "before launched game."
	
	"Zeigt die Gesundheits- und Sicherheits-",					// "Displays the Health and Safety"
	"warnung auf dem unteren Bildschirm an.",					// "message on the bottom screen."
	
	"Aktivieren, wenn Slot-1 Karten auf",						// "Enable this if Slot-1 carts are stuck"
	"einem weißen Bildschirm hängen bleiben.",					// "on white screens."
	
	"Zeigt etwas Text vor dem Spielstart an.",					// "Displays some text before launched game."
	"",															// (empty)

	"Stellt den ARM9 SCFG_EXT fest,",							// "Locks the ARM9 SCFG_EXT,"
	"vermeidet Konflikt mit aktuellem libnds",					// "avoiding conflict with recent libnds."
	
	"Wechsele zwischen offizieller",							// "Change between release and"
	"und inoffizieller Bootstrap Version",						// "unofficial bootstrap file."
	
	/** Start menu **/
	// Options
	"Spielort",													// "Game location"
	"Box-Art: An",												// "Box Art: On"
	"Box-Art: Aus",												// "Box Art: OFF"
	"GBARunner2 starten",										// "Start GBARunner2"
	"Oberer Rand: On",											// "Top border: On"
	"Oberer Rand: Off",											// "Top border: Off"	
	"Entferne Donor ROM",										// "Unset donor rom"	
	"Suche",													// "Search"
	// Values
	"SD Karte",													// "SD Card"
	"Flashkarte",												// "Flashcard"
	"Benutze die Tastatur, um ROMs zu finden",					// "Use the keyboard to find roms"
	
	/** Select menu **/
	// Options
	"ARM9 CPU Geschwindigkeit",									// "ARM9 CPU Speed"
	"VRAM Beschleunigung",										// "VRAM boost"
	"ARM9 SCFG_EXT feststellen",								// "Lock ARM9 SCFG_EXT"
	"Setze als Donor ROM",									// "Set as donor ROM"
	"Setze LED Farbe",											// "Set LED color"
	// Values
	"Standart",													// "Default"
	
	/** Sub-theme **/
	"Unterdesignauswahl: DSi Menu",								// "Sub-theme select: DSi Menu"
	"Unterdesignauswahl: R4",									// "Sub-theme select: R4"
	"Unterdesignauswahl: akMenu/Wood",									// "Sub-theme select: Wood"
	
	"Für dieses Design existiert kein Unterdesign.",			// "No sub-themes exist for this theme."
	
	/** Settings others minor strings **/
	"A/B: Speichern und Zurück",								// "A/B: Save and Return"
	"Links/Rechts: Auswählen",									// "Left/Right: Pick"
	"Einstellungen: GUI",										// "Settings: GUI"
	"Einstellungen: NTR/TWL-mode",								// "Settings: NTR/TWL-mode"

};


#endif /* GERMAN_H */