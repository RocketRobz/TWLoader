#ifndef ITALIAN_H
#define ITALIAN_H

#include <3ds/types.h>

// ITALIAN
static const char *const lang_IT[STR_MAX] = {
	
    /** GUI **/
    "Ritorna al menu HOME",                                       // "Return to HOME Menu"
    "AVVIA",                                                   // "START" (used on cartridge indicator)
    "Nessuna scheda",                                       // "No cartridge"
    "Scheda sconosciuta",                                    // "Unknown Cartridge"
    "Impostazioni",                                               // "Settings"
    "B： Indietro",                                               // "B: Back"   
   
    /** Settings: GUI **/
    "Lingua",                                                   // "Language"
    "Tema",                                                       // "Theme"
    "Colore",                                                   // "Color"
    "Colore del menu",                                         // "Menu Color"
    "Visualizza il nome dei file",                                 // "Show filename"
    "Contatore giochi",                                           // "Game counter"
    "Sfondo inferiore personalizzato",                       // "Custom bottom image"
    "Aggiorna automaticamente bootstrap",                               // "Auto-update bootstrap"
    "Aggiorna automaticamente TWLoader",                           // "Auto-update to latest TWLoader"
	
    /** Settings: NTR/TWL_mode **/
    "Seleziona la Flashcard",                                   // "Flashcard(s) select"
    "LED arcobaleno",                                           // "Rainbow LED"
    "Velocità CPU ARM9",                                       // "ARM9 CPU Speed"
    "VRAM boost",                                               // "VRAM boost"
    "Schermata di avvio DS/DSi",                                       // "DS/DSi Boot Screen"
    "Messaggio Salute e Sicurezza",                               // "Health and Safety message"
    "Resetta lo Slot-1",                                       // "Reset Slot-1"
    "Console output",                                           // "Console output"
    "Blocca ARM9 SCFG_EXT",                                       // "Lock ARM9 SCFG_EXT"
    "Bootstrap",                                               // "Bootstrap"
	
    /** Settings: Top Screen **/
    "X: Aggiorna il bootstrap (Versione ufficiale)",           // "X: Update bootstrap (Official release)"
    "Y: Aggiorna il bootstrap (Versione non ufficiale)",       // "Y: Update bootstrap (Unofficial release)"
    "START: Aggiorna il TWLoader",                               // "START: Update TWLoader"
	
    /** Settings: GUI values **/
    // Color
    "Grigio",                                                   // "Gray"
    "Marrone",                                                   // "Brown"
    "Rosso",                                                   // "Red"
    "Rosa",                                                       // "Pink"
    "Orange",                                                   // "Orange"
    "Giallo",                                                   // "Yellow"
    "Giallo-verde",                                               // "Yellow-Green"
    "Verde 1",                                                   // "Green 1"
    "Verde 2",                                                   // "Green 2"
    "Verde chiaro",                                               // "Light green"
    "Azzurro",                                                   // "Sky blue"
    "Celeste",                                                   // "Light blue"
    "Blu",                                                       // "Blue"
    "Viola",                                                   // "Violet"
    "Porpora",                                                   // "Purple"
    "Fucsia",                                                   // "Fuchsia"
    "Rosso e blu",                                               // "Red and blue"
    "Verde e giallo",                                           // "Green and yellow"
    "Natale",                                                   // "Christmas"
	
    // Menu Color
    "Bianco",                                                   // "White"
    "Nero",                                                       // "Black"
	
    /** Settings: GUI descriptions **/
    "La lingua usata per la UI,",                               // "The language to use for the UI,"
    "incluso il testo dei banner dei giochi.",                   // "including game banner text."	
    "Il tema usato con il TWLoader.",                               // "The theme to use in TWLoader."
    "Premi A per i sotto-temi.",                               // "Press A for sub-themes."
    "Il colore dello sfondo superiore, del bordo",                       // "The color of the top background,"
    "dello START, e dei punti di contorno.",           // "the START border, and the circling dots."
    "Il colore del bordo superiore,",                           // "The color of the top border,"
    "e dello sfondo inferiore.",                               // "and the bottom background."
    "Mosta il nome dei giochi in cima al banner.",               // "Shows game filename at the top of the bubble."
    "",                                                         // (empty)
    "Il numero dei giochi selezionati/elencati ",               // "A number of selected game and listed games"
    "è mostrato sotto il testo del banner.",                   // "is shown below the text bubble."
    "Carica una immagine personalizzata per il ",               // "Loads a custom bottom screen image"
    "menu dei giochi.",                                           // "for the game menu."
    "Aggiorna automaticamente nds-bootstrap ",                   // "Auto-update nds-bootstrap at launch."
    "all'avvio.",                                                         // (empty)
    "Scarica automaticamente l'ultima",                           // "Auto-download the CIA of the latest"
    "versione del CIA TWLoader  all'avvio.",                               // "TWLoader version at launch."
	
    /** Settings: NTR/TWL_mode descriptions **/
    "Seleziona una flashcard da usare",                           // "Pick a flashcard to use to"
    "per avviare le ROM.",                                       // "run ROMs from it."
    "Mosta luci arcobaleno sul",                               // "See rainbow colors glowing in"
    "LED di notifica.",                                           // "the Notification LED."
    "Imposta su TWL per eliminare",                       // "Set to TWL to get rid of lags in some games."
    "il lag in alcuni giochi.",                                 // (empty)
     
    "Abilita la scrittura della VRAM a 8 bit",                   // "Allows 8 bit VRAM writes"
    "ed espande il bus a 32 bit.",                               // "and expands the bus to 32 bit."
    "Mostra la schermata di avvio del DS/DSi",                   // "Displays the DS/DSi boot animation"
    "prima di avviare un gioco.",                               // "before launched game."
    "Mostra l'avviso su Salute e Sicurezza",                   // "Displays the Health and Safety"
    "sullo schermo inferiore.",                                   // "message on the bottom screen."
    "Abilita questa opzione se le schede slot-1",               // "Enable this if Slot-1 carts are stuck"
    "sono bloccate con schermata bianca.",                       // "on white screens."
    "Mosta del testo prima del gioco avviato.",                   // "Displays some text before launched game."
    "",                                                         // (empty)
    "Blocca il SCFG_EXT ARM9,",                                   // "Locks the ARM9 SCFG_EXT,"
    "evitando conflitti con le recenti libnds.",               // "avoiding conflict with recent libnds."
    "Cabia tra versione ufficiale e",                           // "Change between release and"
    "non ufficiale del file di bootstrap.",                       // "unofficial bootstrap file."
	
    /** Start menu **/
    // Options
    "Cartella giochi",                                           // "Game location"
    "Box Art: On",                                              // "Box Art: On"
    "Box Art: Off",                                             // "Box Art: OFF"
    "Avvia GBARunner2",                                         // "Start GBARunner2"
    "Bordo superiore: On",                                          // "Top border: On"
    "Bordo superiore: Off",                                         // "Top border: Off"
    "Non usare ROM donor",                                  // "Unset donor rom"
    "Cerca",                                                   // "Search"
    // Values
    "Scheda SD",                                               // "SD Card"
    "Flashcard",                                               // "Flashcard"
    "Usa la tastiera per trovare le rom",                       // "Use the keyboard to find roms"
	
    /** Select menu **/
    // Options
    "Velocità CPU ARM9",                                       // "ARM9 CPU Speed"
    "VRAM boost",                                               // "VRAM boost"
    "Blocca SCFG_EXT ARM9",                                       // "Lock ARM9 SCFG_EXT"
    "Imposta come ROM donor",                               // "Set as donor ROM"
    "Imposta il colore del LED",                                   // "Set LED color"
    // Values
    "Predefinito",                                               // "Default"
	
    /** Sub-theme **/
    "Seleziona sotto-tema: DSi Menu",                           // "Sub-theme select: DSi Menu"
    "Seleziona sotto-tema: R4",                                   // "Sub-theme select: R4"
    "Seleziona sotto-tema: akMenu/Wood",                               // "Sub-theme select: Wood"
    "Non esistono sotto-temi per questo tema.",                   // "No sub-themes exist for this theme."
	
    /** Settings others minor strings **/
    "A/B: Salva e torna a precedente",                           // "A/B: Save and Return"
    "Left/Right: Seleziona",                                   // "Left/Right: Pick"
    "Impostazioni: GUI",                                            // "Settings: GUI"
    "Impostazioni: NTR/TWL-mode",                                   // "Settings: NTR/TWL-mode"
	
};


#endif /* ITALIAN_H */
