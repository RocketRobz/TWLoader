#ifndef FRENCH_H
#define FRENCH_H
 
#include <3ds/types.h>
 
// FRENCH
static const char *const lang_FR[STR_MAX] = {
     
	/** DS/DSi boot screen **/
	"WARNING - HEALTH AND SAFETY",								// "WARNING - HEALTH AND SAFETY"
	"BEFORE PLAYING, READ THE HEALTH",							// "BEFORE PLAYING, READ THE HEALTH"
	"AND SAFETY PRECAUTIONS BOOKLET",							// "AND SAFETY PRECAUTIONS BOOKLET"
	"FOR IMPORTANT INFORMATION",								// "FOR IMPORTANT INFORMATION"
	"ABOUT YOUR HEALTH AND SAFETY.",							// "ABOUT YOUR HEALTH AND SAFETY."
	"TO GET AN EXTRA COPY FOR YOUR REGION, GO ONLINE AT",		// "TO GET AN EXTRA COPY FOR YOUR REGION, GO ONLINE AT"
	"www.nintendo.com/healthandsafety/",						// "www.nintendo.com/healthandsafety/"
	"",
	
	"Touch the Touch Screen to continue.",						// "Touch the Touch Screen to continue."
	
	/** GUI **/
	": Retourner au Menu HOME",			    // "Return to HOME Menu"
	"START",                                                    // "START" (used on cartridge indicator)
	"Pas de cartouche",                                         // "No cartridge"
	"Paramètres",                                               // "Settings"
	"Cartouche inconnue",                                       // "Unknown Cartridge"
	": Retour",                                               // "B: Back"    
	": Ajouter jeux",					   // "Y: Add Games"
	 
	/** Settings: GUI **/
	"Langue",                                                  // "Language"
	"Thème",                                                   // "Theme"
	"Couleur",                                                 // "Color"
	"Menu des couleurs",                                       // "Menu Color"
	"Afficher le nom du fichier",                              // "Show filename"
	"Compteur de jeux",                                        // "Game counter"
	"Image du bas personnalisée",                              // "Custom bottom image"
	"Mise à jour auto. TWLoader",                              // "Auto-update TWLoader"
	"Type de fichier TWLoader",				   // "TWLoader filetype"
	"Mettre à jour TWLoader",				   // "Update TWLoader"
	"Écran de démarrage DS/DSi",                               // "DS/DSi Boot Screen"
	"Message de Santé et Sécurité",                            // "Health and Safety message"
	"Afficher écran de Boot",				   // "Show Boot Screen"
	"Chemin des ROM",					   // "ROM path"
	 
	/** Settings: NTR/TWL_mode **/
	"Sélection du linker(s)",                                   // "Flashcard(s) select"
	"LED arc-en-ciel",                                          // "Rainbow LED"
	"Vitesse du CPU ARM9",                                      // "ARM9 CPU Speed"
	"Boost VRAM",                                               // "VRAM boost"
	"Reset Slot-1",                                             // "Reset Slot-1"
	"Affichage Console",                                        // "Console output"
	"Mise à jour auto. bootstrap",                        	    // "Auto-update bootstrap"
	"Bootstrap",                                                // "Bootstrap"
	 
	/** Settings: Top Screen **/
	": Mise à jour du bootstrap (release)",        // "X: Update bootstrap (Official release)"
	": Mise à jour du bootstrap (non-officiel)",    // "Y: Update bootstrap (Unofficial release)"
	 
	/** Settings: GUI values **/
	 
	// Color
	"Gris",                                                     // "Gray"
	"Brun",                                                     // "Brown"
	"Rouge",                                                    // "Red"
	"Rose",                                                     // "Pink"
	"Orange",                                                   // "Orange"
	"Jaune",                                                    // "Yellow"
	"Jaune-Vert",                                               // "Yellow-Green"
	"Vert 1",                                                   // "Green 1"
	"Vert 2",                                                   // "Green 2"
	"Vert clair",                                               // "Light green"
	"Bleu ciel",                                                // "Sky blue"
	"Bleu clair",                                               // "Light blue"
	"Bleu",                                                     // "Blue"
	"Violet",                                                   // "Violet"
	"Pourpre",                                                  // "Purple"
	"Fuchsia",                                                  // "Fuchsia"
	"Rouge-bleu",                                               // "Red and blue"
	"Jaune-vert",                                               // "Green and yellow"
	"Noël",                                                	    // "Christmas"

	// Menu Color
	"Blanc",                                                    // "White"
	"Noir",                                                     // "Black"

	/** Settings: GUI descriptions **/
	"La langue à utiliser pour l'UI,",                          // "The language to use for the UI,"
	"incluant le texte des bannières des jeux.",                // "including game banner text."
	 
	"Le thème à utiliser dans TWLoader.",                       // "The theme to use in TWLoader."
	"Appuyez sur START pour les sous-thèmes.",                  // "Press START for sub-themes."
	 
	"La couleur de l'arrière plan du haut,",                    // "The color of the top background,"
	"de la bordure START et du sablier.",            	    // "the START border, and the circling dots."
	 
	"La couleur de la bordure du haut,",                        // "The color of the top border,"
	"et de l'arrière plan du bas.",                             // "and the bottom background."
	 
	"Afficher le nom du fichier du jeu en",  		    // "Shows game filename at the top of the bubble."
	"haut dans la bulle.",                                      // (empty)
	 
	"Le numéro du jeu sélectionné / listé",              	    // "A number of selected game and listed games"
	"est affiché sous la bulle de texte.",                      // "is shown below the text bubble."
	 
	"Charge une image personnalisée sur",        		    // "Loads a custom bottom screen image"
	"l'écran du bas dans le menu des jeux.",                    // "for the game menu."
	 
	"Télécharger automatiquement le dernier",              	    // "Auto-download the CIA of the latest"
	"CIA de TWLoader au démarrage.",                            // "TWLoader version at launch."
	 
	"Où voulez-vour voir l'écran de",			    // "Where do you want to see the"
	"démarrage DS/DSi ?",					    // "DS/DSi boot screen?"

	/** Settings: NTR/TWL_mode descriptions **/
	"Sélectionnez un linker à utiliser pour",                   // "Pick a flashcard to use to"
	"lancer les ROMs.",                                         // "run ROMs from it."
	 
	"Afficher un arc-en-ciel clignotant au",  		    // "See rainbow colors glowing in"
	"niveau de la DEL de notification.",                        // "the Notification LED."
	 
	"Activer pour se débarrasser des",             		    // "Set to TWL to get rid of lags in some games."
	"ralentissements dans certains jeux.",                      // (empty)
	 
	"Permettre l'écriture VRAM 8 bit",                          // "Allows 8 bit VRAM writes"
	"et étendre le bus à 32 bit.",                              // "and expands the bus to 32 bit."
	 
	"Afficher l'animation de démarrage de la",                  // "Displays the DS/DSi boot animation"
	"DS/DSi avant le lancement du jeu.",                        // "before launched game."
	 
	"Afficher le message de Santé et Sécurité",                 // "Displays the Health and Safety"
	"sur l'écran du bas.",                            	    // "message on the bottom screen."
	 
	"Activer si les cartouches Slot-1 sont",           	    // "Enable this if Slot-1 carts are stuck"
	"bloquées sur des écrans blancs.",                          // "on white screens."
	 
	"Aficher du texte avant de lancer un jeu.",                 // "Displays some text before launched game."
	"",                                                         // (empty)
	 
	// "Verrouille le SCFG_EXT de l'ARM9,",                        // "Locks the ARM9 SCFG_EXT,"
	// "évitant les conflits avec la récente libnds.",      	    // "avoiding conflict with recent libnds."
	 
	"Mise à jour auto. de nds-bootstrap",         		    // "Auto-update nds-bootstrap at launch."
	"au démarrage.",                                            // (empty)

	"Changer entre les fichiers de bootstrap",                 // "Pick release, unofficial,"
	"(release non-officiel, et vieux).",                               // "or old bootstrap."
	 
	/** Start menu **/
	// Options
	"Emplacement jeux",                                         // "Game location"
	"Box Art: ON",                                              // "Box Art: On"
	"Box Art: OFF",                                             // "Box Art: OFF"
	"Start GBARunner2",                                         // "Start GBARunner2"
	"Bordure haut: ON",                                         // "Top border: On"
	"Bordure haut: OFF",                                        // "Top border: Off"
	"Déselct. donor ROM",                                       // "Unset donor rom"
	"Recherche",                                                // "Search"
	// Values
	"Carte SD",                                                 // "SD Card"
	"Linker",                                                   // "Flashcard"
	"Utilisez le clavier pour rechercher des roms",             // "Use the keyboard to find roms"
	 
	/** Select menu **/
	// Options
	"Vitesse du CPU ARM9",                                      // "ARM9 CPU Speed"
	"Boost VRAM",                                               // "VRAM boost"
	"Use set donor ROM",                                		// "Use set donor ROM"
	"Définir comme donor ROM",                                  // "Set as donor ROM"
	"Définir la couleur de la DEL",                             // "Set LED color"
	// Values
	"Par défaut",                                               // "Default"
	 
	/** Sub-theme **/
	"Sélection sous-thème: DSi Menu",                           // "Sub-theme select: DSi Menu"
	"Sélection sous-thème: R4",                                 // "Sub-theme select: R4"
	"Sélec. sous-thème: akMenu/Wood",                           // "Sub-theme select: Wood"
	 
	"Aucun autre sous-thème pour ce thème.",                    // "No sub-themes exist for this theme."
	 
	/** Settings others minor strings **/
	"/: Sauvegarder and Retourner",                         // "A/B: Save and Return"
	"Gauche/Droite: Choisir",                                   // "Left/Right: Pick"
	"Paramètres: GUI",                                          // "Settings: GUI"
	"Paramètres: NTR/TWL-mode",                                 // "Settings: NTR/TWL-mode"
 
};
 
 
#endif /* FRENCH_H */
