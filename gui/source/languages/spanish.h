#ifndef SPANISH_H
#define SPANISH_H

#include <3ds/types.h>

// SPANISH
static const char *const lang_ES[STR_MAX] = {
	
	/** GUI **/
    ": Volver al menú HOME",									// "Return to HOME Menu"
	"INICIO",													// "START" (used on cartridge indicator)tor)
	"Sin cartucho",												// "No cartridge"
	"Cartucho desconocido", 									// "Unknown Cartridge"
	"Ajustes",													// "Settings"
	"B: Volver",												// "B: Back"	
	
	/** Settings: GUI **/
	"Idioma",													// "Language"
	"Tema",														// "Theme"
	"Color",													// "Color"
	"Color del menú",											// "Menu Color"
	"Mostrar nombre del archivo",								// "Show filename"
	"Contador de juegos",										// "Game counter"
	"Usar imagen personalizada",								// "Custom bottom image"
	"Auto actualizar bootstrap",								// "Auto-update bootstrap"
	"Auto actualizar TWLoader",									// "Auto-update to latest TWLoader"
	
	/** Settings: NTR/TWL_mode **/
	"Seleccionar flashcard",									// "Flashcard(s) select"
	"LED arcoiris",												// "Rainbow LED"
	"Velocidad del CPU ARM9",									// "ARM9 CPU Speed"
	"Aumentar VRAM",											// "VRAM boost"
	"Pantalla de arranque DS/DSi",								// "DS/DSi Boot Screen"
	"Mensaje de Salud y Seguridad",								// "Health and Safety message"
	"Reiniciar el Slot-1",										// "Reset Slot-1"
	"Salida de consola",										// "Console output"
	"Bloquear el SCFG_EXT del ARM9",							// "Lock ARM9 SCFG_EXT"
	"Bootstrap",												// "Bootstrap"
	
	/** Settings: Top Screen **/
	"X: Actualizar bootstrap (Versión estable)",				// "X: Update bootstrap (Official release)"
	"Y: Actualizar bootstrap (Versión inestable)",				// "Y: Update bootstrap (Unofficial release)"
	"START: Actualizar TWLoader",								// "START: Update TWLoader"
	
	/** Settings: GUI values **/
	
	// Color
	"Gris",														// "Gray"
	"Marrón",													// "Brown"
	"Rojo",														// "Red"
	"Rosa",												 		// "Pink"
	"Naranja",													// "Orange"
	"Amarillo",													// "Yellow"
	"Amarrilo verde",											// "Yellow-Green"
	"Verde 1",													// "Green 1"
	"Verde 2",													// "Green 2"
	"Verde claro",												// "Light green"
	"Azul cielo",												// "Sky blue"
	"Azul claro",												// "Light blue"
	"Azul",														// "Blue"
	"Violeta",													// "Violet"
	"Púrpura",													// "Purple"
	"Fucsia",													// "Fuchsia"
	"Rojo y azul",												// "Red and blue"
	"Verde y amarillo",											// "Green and yellow"
	"Navidad",													// "Christmas"

	// Menu Color
	"Blanco",													// "White"
	"Negro",													// "Black"

	/** Settings: GUI descriptions **/
	"El idioma para usar en TWLoader.",							// "The language to use for the UI,"
	"(también afecta al banner de las roms)",					// "including game banner text."
	
	"El tema para TWLoader.",									// "The theme to use in TWLoader."
	"Pulsa A para elegir un sub tema.",							// "Press A for sub-themes."
	
	"El color del fondo de la pantalla superior, el marco",		// "The color of the top background,"
	"de selección de ROMs y el círculo de puntos.",				// "the START border, and the circling dots."
	
	"Color del marco superior,",								// "The color of the top border,"
	"y del fondo de la pantalla inferior.",						// "and the bottom background."
	
	"Muestra el nombre del archivo en la parte ", 				// "Shows game filename at the top of the bubble."
	"superior de la burbuja de información",					// (empty)
	
	"Muestra el número de la ROM selecionada y el",				// "A number of selected game and listed games"
	"total debajo de la burbuja de información.",				// "is shown below the text bubble."
	
	"Carga una imagen personalizada para",						// "Loads a custom bottom screen image"
	"la pantalla inferior.",									// "for the game menu."
	
	"Auto actualizar bootstrap durante el arranque.",			// "Auto-update nds-bootstrap at launch."
	"",															// (empty) 
	
	"Auto actualizar e installar los CIAS de TWLoader",			// "Auto-download the CIA of the latest"
	"durante el arranque.",										// "TWLoader version at launch."
	
	/** Settings: NTR/TWL_mode descriptions **/
	"Elije una flashcard para cargar",							// "Pick a flashcard to use to"
	"ROMs desde ella.",											// "run ROMs from it."
	
	"Mostrar un efecto arcoiris de colores",					// "See rainbow colors glowing in"
	"en el LED de notificaciones.",								// "the Notification LED."
	
	"Selecciona el modo TWL para eliminar ",					// "Set to TWL to get rid of lags in some games."
	"el lag en algunas ROMs.",									// (empty) 

	"Permite escribir en la VRAM 8 bit",						// "Allows 8 bit VRAM writes"
	"y expandir el bus de datos a 32 bits.",					// "and expands the bus to 32 bit."
	
	"Muestra la animación de arranque de la DS/DSi",			// "Displays the DS/DSi boot animation"
	"antes de ejecutar la ROM.",								// "before launched game."
	
	"Muestra el mensaje de Salud y Seguridad",					// "Displays the Health and Safety"
	"en la pantalla inferior.",									// "message on the bottom screen."
	
	"Activa esta opción para reiniciar el Slot-1",				// "Enable this if Slot-1 carts are stuck"
	"si el cartucho se queda con pantallas blancas.",			// "on white screens."
	
	"Muestra información de depuración",						// "Displays some text before launched game."
	"antes de lanzar la ROM.",									// (empty) 

	"Bloquea el SCFG_EXT del ARM9, evitando",					// "Locks the ARM9 SCFG_EXT,"
	"conflictos con las librerias libnds más recientes.",		// "avoiding conflict with recent libnds."
	
	"Cambia entre la version 'Release' (estable) ",				// "Change between release and"
	"y la 'Unofficial' (inestable).",							// "unofficial bootstrap file."

	/** Start menu **/
	// Options
	"Localización:",											// "Game location"
	"Carátulas: On",											// "Box Art: On"
	"Carátulas: Off",											// "Box Art: OFF"
	"Iniciar GBARunner2",										// "Start GBARunner2"
	"Borde superior: On",										// "Top border: On"
	"Borde superior: Off",										// "Top border: Off"
	"Eliminar donor",											// "Unset donor rom"
	"Buscar",													// "Search"
	// Values
	"Tarjeta SD",												// "SD Card"
	"Flashcard",												// "Flashcard"
	"Usa el teclado para buscar ROMs",							// "Use the keyboard to find roms"
	
	/** Select menu **/
	// Options
	"Velocidad CPU ARM9:",										// "ARM9 CPU Speed"
	"Aumento VRAM:",											// "VRAM boost"
	"Bloq. SCFG_EXT ARM9:",										// "Lock ARM9 SCFG_EXT"
	"Elegir como donor",										// "Set as donor ROM"
	"Elegir color del LED",										// "Set LED color"
	// Values
	"Por defecto",												// "Default"
	
	/** Sub-theme **/
	"Sub tema seleccionado: Menu DSi",							// "Sub-theme select: DSi Menu"
	"Sub tema seleccionado: R4",								// "Sub-theme select: R4"
	"Sub tema seleccionado: akMenu/Wood",						// "Sub-theme select: Wood"
	
	"No hay sub temas para este tema.",							// "No sub-themes exist for this theme."
	
	/** Settings others minor strings **/
	"A/B: Guardar y volver",									// "A/B: Save and Return"
	"Izquierda/Derecha: Seleccionar",							// "Left/Right: Pick"
	"Ajustes: GUI",												// "Settings: GUI"
	"Ajustes: Modo NTR/TWL",									// "Settings: NTR/TWL-mode"

};


#endif /* SPANISH_H */