#ifndef SPANISH_H
#define SPANISH_H

#include <3ds/types.h>

// SPANISH
static const char *const lang_ES[STR_MAX] = {

	/** DS/DSi boot screen **/
	"ADVERTENCIA SOBRE SALUD Y SEGURIDAD",						// "WARNING - HEALTH AND SAFETY"
	"ANTES DE COMENZAR A JUGAR, LEE CON ATENCIÓN EL",			// "BEFORE PLAYING, READ THE HEALTH"
	"FOLLETO DE PRECAUCIONES SOBRE SALUD Y SEGURIDAD,",			// "AND SAFETY PRECAUTIONS BOOKLET"
	"EN EL QUE SE RECOGE INFORMACIÓN IMPORTANTE",				// "FOR IMPORTANT INFORMATION"
	"PARA TU SALUD Y SEGURIDAD.",								// "ABOUT YOUR HEALTH AND SAFETY."
	"PARA OBTENER UNA COPIA ADICIONAL CORRESPONDIENTE",			// "TO GET AN EXTRA COPY FOR YOUR REGION, GO ONLINE AT"
	"A TU ZONA, VISITA ESTA PÁGINA WEB:",
	"www.nintendo.com/healthsafety/",							// "www.nintendo.com/healthsafety/"
	
	"Toca la pantalla táctil para continuar.",					// "Touch the Touch Screen to continue."
	
	/** GUI **/
	": Volver al menú HOME",									// "Return to HOME Menu"
	"INICIAR",													// "START" (used on cartridge indicator)
	"Sin cartucho",												// "No cartridge"
	"Cartucho desconocido", 									// "Unknown Cartridge"
	"Ajustes",													// "Settings"
	": Volver",												// "B: Back"	
	": Añadir juegos",											// "Y: Add Games"

	/** Settings: GUI **/
	"Idioma",													// "Language"
	"Tema",														// "Theme"
	"Color",													// "Color"
	"Color del menú",											// "Menu Color"
	"Nombre del archivo",										// "Show filename"
	"Contador de juegos",										// "Game counter"
	"Imagen personalizada",										// "Custom bottom image"
	"Auto actualiar",											// "Auto-update TWLoader"
	"Tipo de formato",											// "TWLoader filetype"
	"Actualizar",												// "Update TWLoader"
	"Pantalla DS/DSi",											// "DS/DSi Boot Screen"
	"Salud y Seguridad",										// "Health and Safety message"
	"Pantalla de arranque",										// "Show Boot Screen"
	"Rutas",													// "ROM path"
	"Inicio rápido",												// "Quick start"

	/** Settings: NTR_mode **/
	"Seleccionar flashcard",									// "Flashcard(s) select"
	"Notification LED color",									// "Notification LED color"
	"Velocidad del CPU ARM9",									// "ARM9 CPU Speed"
	"Frecuencia de sonido/micrófono",							// "Sound/Microphone frequency"
	"Acceso SD para Slot-1",									// "SD card access for Slot-1"
	"Reinicio del Slot-1",										// "Reset Slot-1"
	"Bootstrap loading screen",									// "Bootstrap loading screen"
	"Salida de consola",										// "Console output"
	"Actualización auto. bootstrap",							// "Auto-update bootstrap"
	"Versión de Bootstrap",										// "Bootstrap"

	/** Settings: Top Screen **/
	": Actualizar bootstrap (Versión oficial)",				// "X: Update bootstrap (Official release)"
	": Actualizar bootstrap (Versión desarrollo)",				// "Y: Update bootstrap (Unofficial release)"

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
	"Gris-Negro",												// "Gray-Black"

	/** Settings: GUI descriptions **/
	"El idioma para usar en TWLoader.",							// "The language to use for the UI,"
	"(también afecta al banner de las roms)",					// "including game banner text."

	"El tema para TWLoader.",									// "The theme to use in TWLoader."
	"Pulsa START para elegir un subtema.",						// "Press START for sub-themes."

	"El color para el fondo superior, el borde",				// "The color of the top background,"
	"de 'Start' y el círculo de puntos.",						// "the START border, and the circling dots."

	"Color del borde superior, y del fondo de",					// "The color of the top border,"
	"la pantalla inferior.",									// "and the bottom background."

	"Muestra el nombre del archivo en la parte", 				// "Shows game filename at the top of the bubble."
	"superior de la burbuja de información.",					// (empty)

	"Muestra el total de ROMs y la posición de",				// "A number of selected game and listed games"
	"la ROM selecionada.",										// "is shown below the text bubble."

	"Carga una imagen personalizada para la",					// "Loads a custom bottom screen image"
	"pantalla inferior.",										// "for the game menu."

	"Descarga e instala automáticamente las",					// "Auto-download the CIA of the latest"
	"actualizaciones durante el arranque.",						// "TWLoader version at launch."

	"Permite mostrar (u ocultar) la pantalla",					// "Where do you want to see the"
	"de DS/DSi.",												// "DS/DSi boot screen?"

	"Durante el arranque, se omiten la descarga",							// "On startup, WiFi features and"
	"WiFi y el almacenamiento en caché de banners.",							// "banner caching will be skipped."

	/** Settings: NTR/TWL_mode descriptions **/
	"Elije una flashcard para cargar ROMs",						// "Pick a flashcard to use to"
	"desde ella.",												// "run ROMs from it."

	"Enciende el LED de notificaciones",						// "See rainbow colors glowing in"
	"haciendo un efecto arcoiris.",								// "the Notification LED."

	"Elije el modo TWL (DSi) para eliminar el",					// "Set to TWL to get rid of lags in some games."
	"lag en algunos juegos",									// (empty) 

	"32.73 kHz: Calidad original",								// "32.73 kHz: Original quality"
	"47.61 kHz: Alta calidad",									// "47.61 kHz: High quality"

	// "Permite escribir en la VRAM 8 bits",					// "Allows 8 bit VRAM writes"
	// "y expandir el bus de datos a 32 bits.",					// "and expands the bus to 32 bit."

	"Permite el acceso a la SD a los cartuchos",				// "Enables SD card access for"
	"del Slot-1.",												// "Slot-1 cards."

	"Muestra la animación de DS/DSi antes de",					// "Displays the DS/DSi boot animation"
	"ejecutar la ROM.",											// "before launched game."

	"Muestra el mensaje de Salud y Seguridad",					// "Displays the Health and Safety"
	"en la pantalla inferior.",									// "message on the bottom screen."

	"Activa esta opción si los cartuchos se",					// "Enable this if Slot-1 carts are stuck"
	"congelan en pantalla blanca.",								// "on white screens."

	"Muestra una pantalla de carga antes de",						// "Shows a loading screen before ROM"
	"la ROM ser cargada en nds-bootstrap.",								// "is started in nds-bootstrap."

	"Muestra información de depuración antes",					// "Displays some text before launched game."
	"de lanzar la ROM.",										// (empty) 

	// "Bloquea el SCFG_EXT del ARM9, evitando",				// "Locks the ARM9 SCFG_EXT,"
	// "conflictos con las librerias libnds más recientes.",	// "avoiding conflict with recent libnds."

	"Actualiza automáticamente bootstrap",						// "Auto-update nds-bootstrap at launch."
	"durante el arranque.",										// (empty) 

	"Cambia entre la version 'Release' (oficial)",				// "Change between release and"
	"y la 'Unofficial' (desarrollo)",							// "unofficial bootstrap file."

	/** Start menu **/
	// Options
	"Localización:",											// "Game location"
	"Carátulas: On",											// "Box Art: On"
	"Carátulas: Off",											// "Box Art: OFF"
	"Select ROM type",											// "Select ROM type"
	"Borde superior: On",										// "Top border: On"
	"Borde superior: Off",										// "Top border: Off"
	"Eliminar donante",											// "Unset donor rom"
	"Buscar",													// "Search"
	// Values
	"Tarjeta SD",												// "SD Card"
	"Flashcard",												// "Flashcard"
	"Usa el teclado para buscar ROMs",							// "Use the keyboard to find roms"

	/** Select ROM type **/
	"Iniciar GBARunner2",										// "Start GBARunner2"

	/** Select menu **/
	// Options
	"Velocidad ARM9:",											// "ARM9 CPU Speed"
	"Acceso SD",												// "SD access"
	"Usar ROM donante",											// "Use set donor ROM"
	"Elegir como donante",										// "Set as donor ROM"
	"Elegir color del LED",										// "Set LED color"
	// Values
	"Por defecto",												// "Default"

	/** Sub-theme **/
	"Sub tema seleccionado: Menu DSi",							// "Sub-theme select: DSi Menu"
	"Sub tema seleccionado: R4",								// "Sub-theme select: R4"
	"Sub tema seleccionado: akMenu/Wood",						// "Sub-theme select: Wood"

	"No hay subtemas para este tema.",							// "No sub-themes exist for this theme."

	/** Settings others minor strings **/
	"/: Guardar y volver",									// "A/B: Save and Return"
	"Izquierda/Derecha: Seleccionar",							// "Left/Right: Pick"
	"Ajustes: GUI",												// "Settings: GUI"
	"Ajustes: Modo NTR",										// "Settings: NTR mode"

};


#endif /* SPANISH_H */
