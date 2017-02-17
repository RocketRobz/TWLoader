#ifndef RUSSIAN_H
#define RUSSIAN_H

#include <3ds/types.h>

// RUSSIAN
static const char *const lang_RU[STR_MAX] = {

	/** GUI **/
  ": Вернуться в меню HOME",									// "Return to HOME Menu"
	"START",													// "START" (used on cartridge indicator)
	"Отсутствует Картридж",												// "No cartridge"
	"Неизвестный Картридж",											// "Unknown Cartridge"
	"Настройки",											// "Settings"
	"B: Назад",												// "B: Back"

	/** Settings: GUI **/
	"Язык",													// "Language"
	"Оформление",													// "Theme"
	"Цвет",													// "Color"
	"Цвет меню",												// "Menu Color"
	"Показать имя файла",										// "Show filename"
	"Игровой счетчик",												// "Game counter"
	"Пользовательское изображение снизу",							// "Custom bottom image"
	"Автоматическое обновление: bootsrap",							// "Auto-update bootstrap"
	"Автоматическое обновление: TWLoader",								// "Auto-update to latest TWLoader"

	/** Settings: NTR/TWL_mode **/
	"Выбор флешкарты",									// "Flashcard(s) select"
	"Радуга LED",											// "Rainbow LED"
	"ARM9 CPU скорость",									// "ARM9 CPU Speed"
	"VRAM ускорение",										// "VRAM boost"
	"DS/DSi стартовый экран",									// "DS/DSi Boot Screen"
	"Сообщение о здоровье и безопасности",				// "Health and Safety message"
	"Перезагрузить Slot-1",										// "Reset Slot-1"
	"Вывод консоли",											// "Console output"
	"Заблокировать ARM9 SCFG_EXT",								// "Lock ARM9 SCFG_EXT"
	"Bootstrap",												// "Bootstrap"

	/** Settings: Top Screen **/
	"X: Обновить Bootstrap (Оффициальная версия)",				// "X: Update bootstrap (Official release)"
	"Y: Обновить Bootstrap (Неоффициальная версия)",				// "Y: Update bootstrap (Unofficial release)"
  "START: Обновить TWLoader",							// "START: Update TWLoader"

	/** Settings: GUI values **/

	// Color
	"Серый",														// "Gray"
	"Коричневый",													// "Brown"
	"Красный",														// "Red"
	"Розовый",												 		// "Pink"
	"Оранжевый",													// "Orange"
	"Желтый",														// "Yellow"
	"Желто-Зеленый",												// "Yellow-Green"
	"Зеленый 1",													// "Green 1"
	"Зеленый 2",													// "Green 2"
	"Светло зеленый",													// "Light green"
	"Небесно голубой",												// "Sky blue"
	"Голубой",													// "Light blue"
	"Синий",														// "Blue"
	"Фиолетовый",													// "Violet"
	"Пурпурный",														// "Purple"
	"Фуксия",													// "Fuchsia"
	"Красный и синий",												// "Red and blue"
	"Зеленый и желтый",											// "Green and yellow"
	"Рождество",												// "Christmas"

	// Menu Color
	"Белый",														// "White"
	"Черный",													// "Black"

	/** Settings: GUI descriptions **/
	"Язый интерфейса,",						// "The language to use for the UI,"
	"включая игровой баннер.",										// "including game banner text."

	"Оформление для TWLoader.",						// "The theme to use in TWLoader."
	"Нажмите A для под-тем оформления.",								// "Press A for sub-themes."

	"Цвет заливки сверху,",						// "The color of the top background,"
	"границ START и круглых точек.",			// "the START border, and the circling dots."

	"Цвет верхней границы",								// "The color of the top border,"
	"и заливки снизу.",							// "and the bottom background."

	"Показать имя файла в пузыре сверху.",		// "Shows game filename at the top of the bubble."
	"",															// (empty)

	"Номер выбранной игры, общее количество игр",				// "A number of selected game and listed games"
	"показаны ниже пузыря с текстом.",	// "is shown below the text bubble."

	"Загрузить собственное изображение для низа",			// "Loads a custom bottom screen image"
	"игрового меню.",								// "for the game menu."

	"Автоматическое обновление nds-bootstrap при старте.",			// "Auto-update nds-bootstrap at launch."
	"",															// (empty)

	"Автоматическая загрузка последнего CIA файла",					// "Auto-download the CIA of the latest"
	"TWLoader при запуске.",								// "TWLoader version at launch."

	/** Settings: NTR/TWL_mode descriptions **/
	"Выберите флешкарту для",							// "Pick a flashcard to use to"
	"запуска ROM-ов с нее.",						// "run ROMs from it."

	"Показывать смену цветов радуги",							// "See rainbow colors glowing in"
	"на LED для уведомлений.",								// "the Notification LED."

	"Установить TWL, чтобы убрать лаги в",						// "Set to TWL to get rid of lags in some games."
	"некоторых играх.",									// (empty)

	"Позволяет делать 8 битные записи VRAM",						// "Allows 8 bit VRAM writes"
	"и расширяет шину до 32 бит.",						// "and expands the bus to 32 bit."

	"Показывать DS/DSi стартовую анимацию",							// "Displays the DS/DSi boot animation"
	"перед запуском игр.",									// "before launched game."

	"Показывать предупреждение о здоровье и",					// "Displays the Health and Safety"
	"безопасности снизу экрана.",					// "message on the bottom screen."

	"Активируйте это если Slot-1 карта",						// "Enable this if Slot-1 carts are stuck"
	"застревает на белом экране.",					// "on white screens."

	"Показывает текст перед запуском игры.",					// "Displays some text before launched game."
	"",															// (empty)

	"Блокировака ARM9 SCFG_EXT позволяет избегать",							// "Locks the ARM9 SCFG_EXT,"
	"конфликтов с последними сборками libnds.",					// "avoiding conflict with recent libnds."

	"Использовать стабильный или неоффициальный",							// "Change between release and"
	"Bootstrap файл или наоборот",						// "unofficial bootstrap file."

	/** Start menu **/
	// Options
	"Расположение игр",													// "Game location"
	"Box-Art: Включить",												// "Box Art: On"
	"Box-Art: Выключить",												// "Box Art: OFF"
	"Запустить GBARunner2",										// "Start GBARunner2"
	"Верхняя граница: Включить",											// "Top border: On"
	"Верхняя граница: Выключить",											// "Top border: Off"
	"Отключить донорский ROM",										// "Unset donor rom"
	"Поиск",													// "Search"
	// Values
	"SD карта",													// "SD Card"
	"Флешкарта",												// "Flashcard"
	"Используйте клавиатуру для поиска ROM-ов",					// "Use the keyboard to find roms"

	/** Select menu **/
	// Options
	"Скорость ARM9 CPU",									// "ARM9 CPU Speed"
	"VRAM ускорение",										// "VRAM boost"
	"Блокировка ARM9 SCFG_EXT",								// "Lock ARM9 SCFG_EXT"
	"Выбрать донорский ROM",									// "Set as donor ROM"
	"Выбрать цвет LED",											// "Set LED color"
	// Values
	"По умолчанию",													// "Default"

	/** Sub-theme **/
	"Выбрать доп-оформление: Меню DSi",								// "Sub-theme select: DSi Menu"
	"Выбрать доп-оформление: R4",									// "Sub-theme select: R4"
	"Выбрать доп-оформление: Дерево",									// "Sub-theme select: Wood"

	"Отсутствует доп-оформление для данной темы оформления.",			// "No sub-themes exist for this theme."

	/** Settings others minor strings **/
	"A/B: Сохранить и вернуться",								// "A/B: Save and Return"
	"Влево/Вправо: Выбрать",									// "Left/Right: Pick"
	"Настройки: GUI",										// "Settings: GUI"
	"Настройки: NTR/TWL-mode",								// "Settings: NTR/TWL-mode"

};


#endif /* RUSSIAN_H */
