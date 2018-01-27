#ifndef FRENCH_H
#define FRENCH_H
 
#include <3ds/types.h>
 
// FRENCH
static const char *const lang_FR[STR_MAX] = {
     
	/** DS/DSi boot screen **/
	"ATTENTION - SANTE ET SECURITE",							// "WARNING - HEALTH AND SAFETY"
	"AVANT DE JOUER, LISEZ LE LIVRET",							// "BEFORE PLAYING, READ THE HEALTH"
	"DE PRECAUTIONS POUR DES INFORMATIONS",						// "AND SAFETY PRECAUTIONS BOOKLET"
	"IMPORTANTES SUR VOTRE SANTE ET SECURITE",					// "FOR IMPORTANT INFORMATION"
	"POUR OBTENIR UN EXEMPLAIRE SUPPLEMENTAIRE",				// "ABOUT YOUR HEALTH AND SAFETY."
	"CORRESPONDANT A VOTRE REGION,",							// "TO GET AN EXTRA COPY FOR YOUR REGION, GO ONLINE AT"
	"CONNECTEZ-VOUS A L'ADRESSE SUIVANTE :",
	"www.nintendo.com/healthsafety/",							// "www.nintendo.com/healthsafety/"
	
	"Touchez l'écran tactile pour continuer.",					// "Touch the Touch Screen to continue."
	
	/** GUI **/
	": Retourner au Menu HOME",			    	   				// "Return to HOME Menu"
	"START",                                                   	// "START" (used on cartridge indicator)
	"Pas de cartouche",                                        	// "No cartridge"
	"Cartouche inconnue",                                      	// "Unknown Cartridge"
	"Paramètres",                                              	// "Settings"	
	":Retour",                                              	// "B: Back"    
	":Ajouter jeux",					  						// "Y: Add Games"
	 
	/** Settings: GUI **/
	"Langue",                                                  	// "Language"
	"Thème",                                                   	// "Theme"
	"Couleur",                                                 	// "Color"
	"Couleur du menu",                                       	// "Menu Color"
	"Aff. nom du fichier",                              		// "Show filename"
	"Compteur de jeux",                                        	// "Game counter"
	"Image du bas perso.",                              		// "Custom bottom image"
	"MàJ auto. TWLoader",                              			// "Auto-update TWLoader"
	"Mode TWLoader",				   							// "TWLoader filetype"
	"MàJ TWLoader",				   								// "Update TWLoader"
	"Écran DS/DSi",                               				// "DS/DSi Boot Screen"
	"Santé et Sécurité",                            			// "Health and Safety message"
	"Aff. écran de Boot",				   						// "Show Boot Screen"
	"Chemin des ROM",					   						// "ROM path"
	"Quick start",												// "Quick start"

	/** Settings: NTR_mode **/
	"Sélection du linker(s)",                                   // "Flashcard(s) select"
	"Notification LED color",									// "Notification LED color"
	"Vitesse du CPU ARM9",                                      // "ARM9 CPU Speed"
	"Fréquence Son/Micro",								// "Sound/Microphone frequency"
	"Accès carte SD pour Slot-1",								// "SD card access for Slot-1"
	"Reset Slot-1",                                             // "Reset Slot-1"
	"Écran de bootstrap",									// "Bootstrap loading screen"
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
	"Gris-Noir",												// "Gray-Black"

	/** Settings: GUI descriptions **/
	"La langue à utiliser pour l'interface,",					// "The language to use for the UI,"
	"incluant le texte des bannières des jeux.",                // "including game banner text."
	 
	"Le thème à utiliser dans TWLoader.",                       // "The theme to use in TWLoader."
	"Appuyez sur START pour les sous-thèmes.",                  // "Press START for sub-themes."
	 
	"La couleur de l'arrière-plan du haut,",                    // "The color of the top background,"
	"de la bordure START et du sablier.",            	    	// "the START border, and the circling dots."
	 
	"La couleur de la bordure du haut,",                        // "The color of the top border,"
	"et de l'arrière-plan du bas.",                             // "and the bottom background."
	 
	"Afficher le nom du fichier du jeu en",  		    		// "Shows game filename at the top of the bubble."
	"haut dans la bulle.",                                      // (empty)
	 
	"Le numéro du jeu sélectionné / listé",              	    // "A number of selected game and listed games"
	"est affiché sous la bulle de texte.",                      // "is shown below the text bubble."
	 
	"Charge une image personnalisée sur",        		    	// "Loads a custom bottom screen image"
	"l'écran du bas dans le menu des jeux.",                    // "for the game menu."
	 
	"Télécharger automatiquement le dernier",              	    // "Auto-download the CIA of the latest"
	"CIA de TWLoader au démarrage.",                            // "TWLoader version at launch."
	 
	"Quand voulez-vous voir l'écran de",			    		// "Where do you want to see the"
	"démarrage DS/DSi ?",					    				// "DS/DSi boot screen?"

	"On startup, WiFi features and",							// "On startup, WiFi features and"
	"banner caching will be skipped.",							// "banner caching will be skipped."

	/** Settings: NTR/TWL_mode descriptions **/
	"Sélectionnez un linker à utiliser pour",                   // "Pick a flashcard to use to"
	"lancer les ROMs.",                                         // "run ROMs from it."
	 
	"Afficher un arc-en-ciel clignotant au",  		    		// "See rainbow colors glowing in"
	"niveau de la DEL de notification.",                        // "the Notification LED."
	 
	"Activer pour se débarrasser des",             		   		// "Set to TWL to get rid of lags in some games."
	"ralentissements dans certains jeux.",                      // (empty)
	 
	"32.73 kHz: Qualité d'origine",								// "32.73 kHz: Original quality"
	"47.61 kHz: Haute qualité",									// "47.61 kHz: High quality"

	// "Permettre l'écriture VRAM 8 bit",                          // "Allows 8 bit VRAM writes"
	// "et étendre le bus à 32 bit.",                              // "and expands the bus to 32 bit."

	"Activer l'accès carte SD pour les",						// "Enables SD card access for"
	"Slot-1 cards.",											// "Slot-1 cards."
	 
	"Afficher l'animation de démarrage de la",                  // "Displays the DS/DSi boot animation"
	"DS/DSi avant le lancement du jeu.",                        // "before launched game."
	 
	"Afficher le message de Santé et Sécurité",                 // "Displays the Health and Safety"
	"sur l'écran du bas.",                            	    	// "message on the bottom screen."
	 
	"Activer si les cartouches Slot-1 sont",           	    	// "Enable this if Slot-1 carts are stuck"
	"bloquées sur des écrans blancs.",                          // "on white screens."
	 
	"Afficher l'écran de chargement avant",						// "Shows a loading screen before ROM"
	"démarrage de la ROM dans nds-bootstrap.",								// "is started in nds-bootstrap."

	"Afficher du texte avant de lancer un jeu.",                 // "Displays some text before launched game."
	"",                                                         // (empty)
	 
	// "Verrouille le SCFG_EXT de l'ARM9,",                        // "Locks the ARM9 SCFG_EXT,"
	// "évitant les conflits avec la récente libnds.",      	    // "avoiding conflict with recent libnds."
	 
	"Mise à jour auto. de nds-bootstrap",         		    	// "Auto-update nds-bootstrap at launch."
	"au démarrage.",                                            // (empty)

	"Choisir le fichier de bootstrap",							// "Pick release or unofficial bootstrap."
	"(Release ou Unofficial).",									// (empty)
	 
	/** Start menu **/
	// Options
	"Emplacement jeux",                                         // "Game location"
	"Box Art: ON",                                              // "Box Art: On"
	"Box Art: OFF",                                             // "Box Art: OFF"
	"Select ROM type",											// "Select ROM type"
	"Bordure haut: ON",                                         // "Top border: On"
	"Bordure haut: OFF",                                        // "Top border: Off"
	"Déselct. donor ROM",                                       // "Unset donor rom"
	"Rechercher…",                                            	// "Search"
	// Values
	"Carte SD",                                                 // "SD Card"
	"Linker",                                                   // "Flashcard"
	"Utilisez le clavier pour rechercher des ROMs",             // "Use the keyboard to find roms"

	/** Select ROM type **/
	"Start GBARunner2",											// "Start GBARunner2"

	/** Select menu **/
	// Options
	"Fréq. du CPU ARM9",                                      	// "ARM9 CPU Speed"
	"Accès SD",													// "SD access"
	"Utiliser donor ROM",                                		// "Use set donor ROM"
	"Définir donor ROM",                                  		// "Set as donor ROM"
	"Couleur de la DEL",                             			// "Set LED color"
	// Values
	"Par défaut",                                               // "Default"
	 
	/** Sub-theme **/
	"Sélection sous-thème: DSi Menu",                           // "Sub-theme select: DSi Menu"
	"Sélection sous-thème: R4",                                 // "Sub-theme select: R4"
	"Sélec. sous-thème: akMenu/Wood",                           // "Sub-theme select: Wood"
	 
	"Aucun sous-thème pour ce thème.",                    		// "No sub-themes exist for this theme."
	 
	/** Settings others minor strings **/
	"/: Sauvegarder et Retourner",                         	// "A/B: Save and Return"
	"Gauche/Droite: Choisir",                                   // "Left/Right: Pick"
	"Paramètres: GUI",                                          // "Settings: GUI"
	"Paramètres: NTR mode",										// "Settings: NTR mode"
 
};
 
 
#endif /* FRENCH_H */
