#ifndef RUSSIAN_H
#define RUSSIAN_H

#include <3ds/types.h>

// RUSSIAN
static const char *const lang_RU[STR_MAX] = {

	/** DS/DSi boot screen **/
	"ВНИМАНИЕ - ТЕХНИКА БЕЗОПАСНОСТИ",							// "WARNING - HEALTH AND SAFETY"
	"ПЕРЕД ИГРОЙ, ПРОЧТИТЕ БУКЛЕТ",								// "BEFORE PLAYING, READ THE HEALTH"
	"О ТЕХНИКЕ БЕЗОПАСНОСТИ",									// "AND SAFETY PRECAUTIONS BOOKLET"
	"ДЛЯ ТОГО, ЧТОБЫ БОЛЬШЕ",									// "FOR IMPORTANT INFORMATION"
	"УЗНАТЬ О НЕЙ.",											// "ABOUT YOUR HEALTH AND SAFETY."
	"ДЛЯ ДОПОЛНИТЕЛЬНОЙ ИНФОРМАЦИИ, ЗАЙДИТЕ НА",				// "TO GET AN EXTRA COPY FOR YOUR REGION, GO ONLINE AT"
	"www.nintendo.com/healthsafety/",							// "www.nintendo.com/healthsafety/"
	"",
	
	"Коснитесь экрана, чтобы продолжить.",						// "Touch the Touch Screen to continue."
	
	/** GUI **/
	": Вернуться в меню HOME",									// "Return to HOME Menu"
	"СТАРТ",													// "START" (used on cartridge indicator)
	"Картридж отсутствует",										// "No cartridge"
	"Неизвестный картридж",										// "Unknown Cartridge"
	"Настройки",												// "Settings"
	": Назад",													// "B: Back"
	": Доб-ть игры",											// "Y: Add Games"

	/** Settings: GUI **/
	"Язык",														// "Language"
	"Оформление",												// "Theme"
	"Цвет",														// "Color"
	"Цвет меню",												// "Menu Color"
	"Показать имя файла",										// "Show filename"
	"Счётчик игр",												// "Game counter"
	"Своё нижнее изобр.",										// "Custom bottom image"
	"Автообнов. TWLoader",										// "Auto-update TWLoader"
	"Файл TWLoader",											// "TWLoader filetype"
	"Обновить TWLoader",										// "Update TWLoader"
	"Экран DS/DSi",												// "DS/DSi Boot Screen"
	"Экран Техники без-ти",										// "Health and Safety message"
	"Старт. экран",												// "Show Boot Screen"
	"Путь к ROM-у",												// "ROM path"

	/** Settings: NTR_mode **/
	"Выбор флешкарты",											// "Flashcard(s) select"
	"Радуга LED",												// "Rainbow LED"
	"Скорость ЦП ARM9",											// "ARM9 CPU Speed"
	"Sound/Microphone frequency",								// "Sound/Microphone frequency"
	"Доступ SD-карты для Slot-1",								// "SD card access for Slot-1"
	"Перезагрузить Slot-1",										// "Reset Slot-1"
	"Bootstrap loading screen",									// "Bootstrap loading screen"
	"Вывод консоли",											// "Console output"
	"Автообновление бутстрапа",									// "Auto-update bootstrap"
	"Бутстрап",													// "Bootstrap"

	/** Settings: Top Screen **/
	": Обновить бутстрап (Оффициальная версия)",				// "X: Update bootstrap (Official release)"
	": Обновить бутстрап (Неоффициальная версия)",				// "Y: Update bootstrap (Unofficial release)"

	/** Settings: GUI values **/

	// Color
	"Серый",													// "Gray"
	"Коричневый",												// "Brown"
	"Красный",													// "Red"
	"Розовый",												 	// "Pink"
	"Оранжевый",												// "Orange"
	"Желтый",													// "Yellow"
	"Желто-Зеленый",											// "Yellow-Green"
	"Зеленый 1",												// "Green 1"
	"Зеленый 2",												// "Green 2"
	"Светло зеленый",											// "Light green"
	"Небесно голубой",											// "Sky blue"
	"Голубой",													// "Light blue"
	"Синий",													// "Blue"
	"Фиолетовый",												// "Violet"
	"Пурпурный",												// "Purple"
	"Фуксия",													// "Fuchsia"
	"Красный и синий",											// "Red and blue"
	"Зеленый и желтый",											// "Green and yellow"
	"Рождество",												// "Christmas"

	// Menu Color
	"Белый",													// "White"
	"Черный",													// "Black"

	/** Settings: GUI descriptions **/
	"Язый интерфейса,",											// "The language to use for the UI,"
	"включая игровой баннер.",									// "including game banner text."

	"Оформление для TWLoader.",									// "The theme to use in TWLoader."
        "Нажмите START для выбора под-тем.",					// "Press START for sub-themes."

	"Цвет заливки сверху,",										// "The color of the top background,"
	"кнопки START и круглых точек.",							// "the START border, and the circling dots."

	"Цвет верхней границы",										// "The color of the top border,"
	"и заливки снизу.",											// "and the bottom background."

	"Показать имя файла в пузыре сверху.",						// "Shows game filename at the top of the bubble."
	"",															// (empty)

	"Номер выбранной игры и общее число игр",					// "A number of selected game and listed games"
	"будут показаны под текстом.",								// "is shown below the text bubble."

	"Загрузить своё изображение для низа",						// "Loads a custom bottom screen image"
	"игрового меню.",											// "for the game menu."

	"Автомат-ая загрузка посл-го CIA",							// "Auto-download the CIA of the latest"
	"TWLoader при запуске.",									// "TWLoader version at launch."

	"Когда вы хотите видеть",									// "Where do you want to see the"
	"экран запуска DS/DSi?",									// "DS/DSi boot screen?"

	/** Settings: NTR/TWL_mode descriptions **/
	"Выберите флешкарту для",									// "Pick a flashcard to use to"
	"запуска ROM-ов с нее.",									// "run ROMs from it."

	"Показывать смену цветов радуги",							// "See rainbow colors glowing in"
	"на уведомляющем-LED.",										// "the Notification LED."

	"Выберите TWL, чтобы убрать лаги в",						// "Set to TWL to get rid of lags in some games."
	"некоторых играх.",											// (empty)

	"32.73 kHz: Original quality",								// "32.73 kHz: Original quality"
	"47.61 kHz: High quality",									// "47.61 kHz: High quality"

	// "Позволяет делать 8 битные записи VRAM",					// "Allows 8 bit VRAM writes"
	// "и расширяет шину до 32 бит.",							// "and expands the bus to 32 bit."

	"Разрешить доступ к SD-карте",								// "Enables SD card access for"
	"для картриджей Slot-1.",									// "Slot-1 cards."

	"Показывать стартовый экран",								// "Displays the DS/DSi boot animation"
	"DS/DSi перед запуском игр.",								// "before launched game."

	"Показывать текст техники",									// "Displays the Health and Safety"
	"безопасности снизу экрана.",								// "message on the bottom screen."

	"Включите, если картридж Slot-1",							// "Enable this if Slot-1 carts are stuck"
	"застревает на белом экране.",								// "on white screens."

	"Shows a loading screen before ROM",						// "Shows a loading screen before ROM"
	"is started in nds-bootstrap.",								// "is started in nds-bootstrap."

	"Показывает текст перед запуском",							// "Displays some text before launched game."
	"игры.",													// (empty)

	// "Блокировака ARM9 SCFG_EXT позволяет избегать",			// "Locks the ARM9 SCFG_EXT,"
	// "конфликтов с последними сборками libnds.",				// "avoiding conflict with recent libnds."

	"Автообновление nds-bootstrap",								// "Auto-update nds-bootstrap at launch."
	"при старте.",												// (empty)

	"Исп-ть стабильный/нестабильный",							// "Change between release and"
	"файл бутстрапа.",											// "unofficial bootstrap file."

	/** Start menu **/
	// Options
	"Расположение игр",											// "Game location"
	"Обложка: Вкл",												// "Box Art: On"
	"Обложка: Выкл",											// "Box Art: OFF"
	"Запуск GBARunner2",										// "Start GBARunner2"
	"Оверлей: Вкл",												// "Top border: On"
	"Оверлей: Выкл",											// "Top border: Off"
	"Не исп. ROM-донор",										// "Unset donor rom"
	"Поиск",													// "Search"
	// Values
	"SD карта",													// "SD Card"
	"Флешкарта",												// "Flashcard"
	"Используйте клавиатуру для поиска ROM-ов",					// "Use the keyboard to find roms"

	/** Select menu **/
	// Options
	"Скорость ЦП ARM9",											// "ARM9 CPU Speed"
	"Доступ к SD",												// "SD access"
	"Исп-ть ROM-донор",											// "Use set donor ROM"
	"Выбрать ROM-донор",										// "Set as donor ROM"
	"Выбрать цвет LED",											// "Set LED color"
	// Values
	"По умол-ию",												// "Default"

	/** Sub-theme **/
	"Выбрать доп-оформление: Меню DSi",							// "Sub-theme select: DSi Menu"
	"Выбрать доп-оформление: R4",								// "Sub-theme select: R4"
	"Выбрать доп-оформление: Дерево",							// "Sub-theme select: Wood"

	"Отсутствует доп-оформление для данной темы оформления.",	// "No sub-themes exist for this theme."

	/** Settings others minor strings **/
	"/: Сохр. и вернуться",									// "A/B: Save and Return"
	"Влево/Вправо: Выбрать",									// "Left/Right: Pick"
	"Настройки интерфейса",										// "Settings: GUI"
	"Настройки NTR",											// "Settings: NTR mode"

};


#endif /* RUSSIAN_H */