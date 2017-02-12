#ifndef FRENCH_H
#define FRENCH_H
#include <3ds/types.h>
// FRENCH
static const char *const lang_FR[STR_MAX] = {

/** GUI **/
": Retourner to HOME Menu", // "Return to HOME Menu"
"Démar", // "START" (used on cartridge indicator)
"Pas de cartouche", // "No cartridge"
"Paramètres", // "Settings"
"Cartouche inconnue" // "Unknown Cartridge"
"B: Retourer", // "B: Back" 

/** Settings: GUI **/
"Langue", // "Language"
"Thème", // "Theme"
"Couleur", // "Color"
"Menu des couleurs", // "Menu Color"
"Afficher le nom du fichier", // "Show filename"
"Conteur de jeu", // "Game counter"
"Image du bas personnalisée", // "Custom bottom image"
"Mise à jour auto. de la bootstrap", // "Auto-update bootstrap"
"Mise à jour auto. de TWLoader", // "Auto-update to latest TWLoader"

/** Settings: NTR/TWL_mode **/
"Sélection du linker(s)", // "Flashcard(s) select"
"LED arc-en-ciel", // "Rainbow LED"
"Vitesse du CPU ARM9", // "ARM9 CPU Speed"
"Boost VRAM", // "VRAM boost"
"Écran de démarrage DS/DSi", // "DS/DSi Boot Screen"
"Message de Santé et Sécurité", // "Health and Safety message"
"Re-lancé Slot-1", // "Reset Slot-1"
"Affichage", // "Console output"
"Vérouiller SCFG_EXT ARM9", // "Lock ARM9 SCFG_EXT"
"Bootstrap", // "Bootstrap"

/** Settings: Top Screen **/
"X: Mise à jour de la bootstrap (Release officiel)", // "X: Update bootstrap (Official release)"
"X: Mise à jour de la bootstrap (Release non-officiel)", // "Y: Update bootstrap (Unofficial release)"
"START: Mettre à jour TWLoader", // "START: Update TWLoader"

/** Settings: GUI values **/

// Color
"Gris", // "Gray"
"Brun", // "Brown"
"Rouge", // "Red"
"Rose", // "Pink"
"Orange", // "Orange"
"Jaune", // "Yellow"
"Jaune-Vert", // "Yellow-Green"
"Vert 1", // "Green 1"
"Vert 2", // "Green 2"
"Vert lumière", // "Light green"
"Bleu ciel", // "Sky blue"
"Bleu lumière", // "Light blue"
"Bleu", // "Blue"
"Violet", // "Violet"
"Pourpre", // "Purple"
"Fuchsia", // "Fuchsia"
"Rouge&Bleu", // "Red and blue"
"Jaune&Vert", // "Green and yellow"
"Noêl", // "Christmas"
// Menu Color
"Blanc", // "White"
"Noir", // "Black"
/** Settings: GUI descriptions **/
"La langue à utiliser pour l'UI,", // "The language to use for the UI,"
"incluant le texte des bannières des jeux.", // "including game banner text."

"Le thème à utiliser dans TWLoader.", // "The theme to use in TWLoader."
"Appuyez sur A pour les sous-thèmes.", // "Press A for sub-themes."

"La couleur de l'arrière plan du haut,", // "The color of the top background,"
"de la bordure START et du contour des points.", // "the START border, and the circling dots."

"La couleur de la bordure du haut,", // "The color of the top border,"
"et de l'arrière plan du bas.", // "and the bottom background."

"Shows game filename at the top of the bubble.", // "Shows game filename at the top of the bubble."
"", // (empty)

"Une numbre de jeus selaction", // "A number of selected game and listed games"
"is shown below the text bubble.", // "is shown below the text bubble."

"Loads a custom bottom screen image", // "Loads a custom bottom screen image"
"for the game menu.", // "for the game menu."

"Auto-update nds-bootstrap at launch.", // "Auto-update nds-bootstrap at launch."
"", // (empty)
"Auto-download the CIA of the latest", // "Auto-download the CIA of the latest"
"TWLoader version at launch.", // "TWLoader version at launch."

/** Settings: NTR/TWL_mode descriptions **/
"Pick a flashcard to use to", // "Pick a flashcard to use to"
"run ROMs from it.", // "run ROMs from it."

"Vie colour rainbow dans", // "See rainbow colors glowing in"
"le LED du Notification.", // "the Notification LED."

"Set to TWL denlever lags dans jeus.", // "Set to TWL to get rid of lags in some games."
"", // (empty)

"Allows 8 bit VRAM writes", // "Allows 8 bit VRAM writes"
"et expands le bus a 32 bit.", // "and expands the bus to 32 bit."

"Displays the DS/DSi boot animation", // "Displays the DS/DSi boot animation"
"before launched game.", // "before launched game."

"Displays the Health and Safety", // "Displays the Health and Safety"
"message on the bottom screen.", // "message on the bottom screen."

"Enable this if Slot-1 carts are stuck", // "Enable this if Slot-1 carts are stuck"
"on white screens.", // "on white screens."

"Displays some text before launched game.", // "Displays some text before launched game."
"", // (empty)

"Locks the ARM9 SCFG_EXT,", // "Locks the ARM9 SCFG_EXT,"
"avoiding conflict with recent libnds.", // "avoiding conflict with recent libnds."

"Change between release and", // "Change between release and"
"unofficial bootstrap file.", // "unofficial bootstrap file."

/** Start menu **/
// Options
"Localisation des jeux", // "Game location"
"Box Art: Activer", // "Box Art: On"
"Box Art: Désactiver", // "Box Art: OFF"
"Démarrer GBARunner2", // "Start GBARunner2"
"Bordure du haut: Activer", // "Top border: On"
"Bordure du haut: Désactiver", // "Top border: Off"
"Unset donor rom", // "Unset donor rom"
"Recherche", // "Search"
// Values
"Carte SD", // "SD Card"
"Linker", // "Flashcard"
"Utiliser le clavier pour trouver des roms", // "Use the keyboard to find roms"

/** Select menu **/
// Options
"Vitesse du CPU ARM9", // "ARM9 CPU Speed"
"Boost VRAM", // "VRAM boost"
"Vérouiller SCFG_EXT ARM9", // "Lock ARM9 SCFG_EXT"
"Définir comme ROM donneur", // "Set as donor ROM"
"Définir la couleur de la LED", // "Set LED color"
// Values
"Défaut", // "Default"

/** Sub-theme **/
"Sélection du sous-thème: DSi Menu", // "Sub-theme select: DSi Menu"
"Sélection du sous-thème: R4", // "Sub-theme select: R4"
"Sélection du sous-thème: Wood", // "Sub-theme select: Wood"

"Aucune autre sous-thèmes pour ce thème.", // "No sub-themes exist for this theme."

/** Settings others minor strings **/
"A/B: Sauvegarder and Retourner", // "A/B: Save and Return"
"Gauche/Droite: Choisir", // "Left/Right: Pick"
"Paramètres: GUI", // "Settings: GUI"
"Paramètres: Mode NTR/TWL", // "Settings: NTR/TWL-mode"
};
#endif /* FRENCH_H */
