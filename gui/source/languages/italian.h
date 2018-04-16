#ifndef ITALIAN_H
#define ITALIAN_H

#include <3ds/types.h>

// ITALIAN
static const char *const lang_IT[STR_MAX] = {

	/** DS/DSi boot screen **/
	"ATTENZIONE-SALUTE E SICUREZZA",							// "WARNING - HEALTH AND SAFETY"
	"PRIMA DI INIZIARE A GIOCARE, LEGGI ATTENTAMENTE",			// "BEFORE PLAYING, READ THE HEALTH"
	"IL LIBRETTO DI PRECAUZIONI PER LA SALUTE E LA",			// "AND SAFETY PRECAUTIONS BOOKLET"
	"SICUREZZA, DOVE TROVI IMPORTANTI INFORMAZIONI",			// "FOR IMPORTANT INFORMATION"
	"PER LA TUA SALUTE E SICUREZZA.",							// "ABOUT YOUR HEALTH AND SAFETY."
	"PER OTTENERNE UNA COPIA SUPPLEMENTARE RELATIVA",			// "TO GET AN EXTRA COPY FOR YOUR REGION, GO ONLINE AT"
	"ALLA TUA AREA VAI ALL'INDIRIZZO INTERNET:",
	"www.nintendo.com/healthsafety/",							// "www.nintendo.com/healthsafety/"
	
	"Tocca il touch screen per continuare.",					// "Touch the Touch Screen to continue."
	
	/** GUI **/
	"Ritorna al menu HOME",                                 	// "Return to HOME Menu"
	"AVVIA",                                               	 	// "START" (used on cartridge indicator)
	"Nessuna scheda",                                      	 	// "No cartridge"
	"Scheda sconosciuta",                                   	// "Unknown Cartridge"
	"Impostazioni",                                         	// "Settings"
	": Chiudi",                                             	// "B: Back"  
	": Aggiungi giochi",												// "Y: Add Games"

	/** Settings: GUI **/
	"Lingua",                                               	// "Language"
	"Tema",                                                	 	// "Theme"
	"Colore",                                               	// "Color"
	"Colore del menu",                                      	// "Menu Color"
	"Visualizza nome file",                          	// "Show filename"
	"Contatore giochi",                                    		// "Game counter"
	"Sfondo inf. personalizz.",                    		// "Custom bottom image"
	"Auto-aggiorna TWLoader",             	       	// "Auto-update TWLoader"
	"Tipo file TWLoader",										// "TWLoader filetype"
	"Aggiorna TWLoader",										// "Update TWLoader"
	"Schermata avvio DS/DSi",                                // "DS/DSi Boot Screen"
	"Messaggio S&S",                             // "Health and Safety message"
	"Mostra scherm. avvio",											// "Show Boot Screen"
	"Cartella ROM",													// "ROM path"
	"Avvio rapido",												// "Quick start"

	/** Settings: NTR_mode **/
	"Seleziona la flashcard",                                   // "Flashcard(s) select"
	"Colore LED di notifica",									// "Notification LED color"
	"Velocità CPU ARM9",                                       	// "ARM9 CPU Speed"
	"Frequenza suono/microfono",								// "Sound/Microphone frequency"
	"Accesso SD per Slot-1",								// "SD card access for Slot-1"
	"Resetta lo Slot-1",                                       	// "Reset Slot-1"
	"Scherm. caric. bootstrap",									// "Bootstrap loading screen"
	"Console output",                                           // "Console output"
	"Auto-aggiorna bootstrap",             		    // "Auto-update bootstrap"
	"Versione bootstrap",                                               	// "Bootstrap"

	/** Settings: Top Screen **/
	": Aggiorna bootstrap (ufficiale)",           	// "X: Update bootstrap (Official release)"
	": Aggiorna bootstrap (non ufficiale)",       	// "Y: Update bootstrap (Unofficial release)"

	/** Settings: GUI values **/
	
	// Color
	"Grigio",                                                   // "Gray"
	"Marrone",                                                 	// "Brown"
	"Rosso",                                                   	// "Red"
	"Rosa",                                                    	// "Pink"
	"Arancio",                                                   // "Orange"
	"Giallo",                                                   // "Yellow"
	"Giallo-verde",                                             // "Yellow-Green"
	"Verde 1",                                                  // "Green 1"
	"Verde 2",                                                  // "Green 2"
	"Verde chiaro",                                             // "Light green"
	"Azzurro",                                                  // "Sky blue"
	"Celeste",                                                  // "Light blue"
	"Blu",                                                      // "Blue"
	"Viola",                                                   	// "Violet"
	"Porpora",                                                  // "Purple"
	"Fucsia",                                                   // "Fuchsia"
	"Rosso e blu",                                              // "Red and blue"
	"Verde e giallo",                                           // "Green and yellow"
	"Natalizio",                                                   // "Christmas"

	// Menu Color
	"Bianco",                                                   // "White"
	"Grigio-Nero",												// "Gray-Black"

	/** Settings: GUI descriptions **/
	"La lingua usata per la UI,",                               // "The language to use for the UI,"
	"incluso il testo dei banner dei giochi.",                  // "including game banner text."

	"Il tema usato con il TWLoader.",                           // "The theme to use in TWLoader."
	"Premi START per i sotto-temi.",                            // "Press START for sub-themes."

	"Il colore dello sfondo superiore, del bordo",              // "The color of the top background,"
	"dello START e dei punti di contorno.",           			// "the START border, and the circling dots."

	"Il colore del bordo superiore",                           // "The color of the top border,"
	"e dello sfondo inferiore.",                               	// "and the bottom background."

	"Mostra il nome dei giochi in cima",              // "Shows game filename at the top of the bubble."
	"al banner.",                                                         // (empty)

	"Il numero dei giochi selezionati/elencati ",               // "A number of selected game and listed games"
	"è mostrato sotto il testo del banner.",                   	// "is shown below the text bubble."

	"Carica una immagine personalizzata per il",               // "Loads a custom bottom screen image"
	"menu del DSi.",                                 	        // "for the DSi Menu theme."

	"Scarica automaticamente l'ultima",                         // "Auto-download the CIA of the latest"
	"versione di TWLoader all'avvio.",                    // "TWLoader version at launch."

	"Dove vuoi vedere la schermata di",								// "Where do you want to see the"
	"avvio DS/DSi?",										// "DS/DSi boot screen?"

	"All'avvio, saranno saltate le funzioni",							// "On startup, WiFi features and"
	"Wi-Fi e di cache dei banner.",							// "banner caching will be skipped."

	/** Settings: NTR/TWL_mode descriptions **/
	"Seleziona una flashcard da usare",                         // "Pick a flashcard to use to"
	"per avviare le ROM.",                                      // "run ROMs from it."

	"Seleziona il colore da mostrare",                               	// "See rainbow colors glowing in"
	"sul LED di notifica.",                                         // "the Notification LED."

	"Imposta su TWL per eliminare",                       		// "Set to TWL to get rid of lags in some games."
	"il lag in alcuni giochi.",                                 // (empty)

	"32.73 kHz: Qualità originale",								// "32.73 kHz: Original quality"
	"47.61 kHz: Alta qualità",									// "47.61 kHz: High quality"

	// "Abilita la scrittura della VRAM a 8 bit",                  // "Allows 8 bit VRAM writes"
	// "ed espande il bus a 32 bit.",                              // "and expands the bus to 32 bit."

	"Abilita l'accesso alla SD per",								// "Enables SD card access for"
	"le schede Slot-1.",											// "Slot-1 cards."

	"Mostra la schermata di avvio del DS/DSi",                  // "Displays the DS/DSi boot animation"
	"prima di avviare un gioco.",                               // "before launched game."

	"Mostra l'avviso su Salute e Sicurezza",                   	// "Displays the Health and Safety"
	"sullo schermo inferiore.",                                 // "message on the bottom screen."

	"Abilita questa opzione se le schede slot-1",               // "Enable this if Slot-1 carts are stuck"
	"sono bloccate con schermata bianca.",                      // "on white screens."

	"Aggiunge una schermata di caricamento",						// "Shows a loading screen before ROM"
	"della ROM in nds-bootstrap.",								// "is started in nds-bootstrap."

	"Mostra del testo durante l'avvio del",                 // "Displays some text before launched game."
	"gioco.",                                                         // (empty)

	// "Blocca il SCFG_EXT ARM9,",                                 // "Locks the ARM9 SCFG_EXT,"
	// "evitando conflitti con le recenti libnds.",               	// "avoiding conflict with recent libnds."

	"Aggiorna automaticamente nds-bootstrap",                  // "Auto-update nds-bootstrap at launch."
	"all'avvio.",                                               // (empty)

	"Scegli tra versione ufficiale (Release) e",                           // "Change between release and"
	"non ufficiale del file di bootstrap.",                     // "unofficial bootstrap file."

	/** Start menu **/
	// Options
	"Cartella giochi",                                          // "Game location"
	"Copertina: On",                                              // "Box Art: On"
	"Copertina: Off",                                             // "Box Art: OFF"
	"Seleziona tipo ROM",											// "Select ROM type"
	"Bordo superiore: On",                                      // "Top border: On"
	"Bordo superiore: Off",                                     // "Top border: Off"
	"Resetta ROM donor",                                     // "Unset donor rom"
	"Cerca",                                                   	// "Search"
	// Values
	"Scheda SD",                                               	// "SD Card"
	"Flashcard",                                               	// "Flashcard"
	"Cerca una ROM...",                       // "Use the keyboard to find roms"

	/** Select ROM type **/
	"Avvia GBARunner2",                                         // "Start GBARunner2"

	/** Select menu **/
	// Options
	"Velocità CPU ARM9",                                       	// "ARM9 CPU Speed"
	"Accesso SD",												// "SD access"
	"Usa ROM donor",                                     	// "Use set donor ROM"
	"Imposta come donor",                               	// "Set as donor ROM"
	"Imposta colore LED",                                   // "Set LED color"
	// Values
	"Predefinito",                                              // "Default"

	/** Sub-theme **/
	"Seleziona sotto-tema: DSi Menu",                           // "Sub-theme select: DSi Menu"
	"Seleziona sotto-tema: R4",                                 // "Sub-theme select: R4"
	"Seleziona sotto-tema: akMenu/Wood",                        // "Sub-theme select: Wood"
	
	"Non esistono sotto-temi per questo tema.",                 // "No sub-themes exist for this theme."

	/** Settings others minor strings **/
	"/: Salva e torna a precedente",                          // "A/B: Save and Return"
	"Left/Right: Seleziona",                                   	// "Left/Right: Pick"
	"Impostazioni: GUI",                                        // "Settings: GUI"
	"Impostazioni: NTR mode",                               	// "Settings: NTR mode"

};


#endif /* ITALIAN_H */
