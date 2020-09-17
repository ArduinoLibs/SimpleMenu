// подключаем библиотеки
// библиотеки GyverEncoder, GyverOLED и т.п библиотеки для ввода/вывода данных через меню
// должны быть подключены до библиотеки SimpleMenu
#include <GyverEncoder.h>
#include <GyverOLED.h>
#include <SimpleMenu.h>

// создаём для примера меню и под-меню
// меню будет управляться с кнопки и выводиться нами, через нашу функцию
// устанавливаем нужные флажки
SimpleMenu menu(SMENU_INPUT_ENCODER, SMENU_OUTPUT_OLED);
// а под-меню будет управляться из первого меню
SimpleMenu submenu(SMENU_INPUT_SUBMENU, SMENU_INPUT_SUBMENU);

// создадим переменные с энкодером
#define CLK 6
#define DT 5
#define SW 4
Encoder enc(CLK, DT, SW);

// Обьект дисплея
GyverOLED oled;

// the setup function runs once when you press reset or power the board
void setup() {
  // настроим наш OLED дисплей
  oled.init(OLED128x64, 500);
  oled.setContrast(255);
  // установим для пина 13 режим OUTPUT
  // это светодиод на плате
  pinMode(LED_BUILTIN, OUTPUT);
  // открываем порт для общения с ПК (скорость 57600 чтобы всё выводилось быстрее, а то когда 10 строк выводятся одна за другой - не круто)
  Serial.begin(57600);
  // так как мы настроили что вывод данных у нас будет на OLED, говорим библиотеке тип экрана и указатель на него
  // для примера будем использовать 8
  menu.initOLED(OLED128x64, &oled);
  // установим "плавный скрол" для первого меню, когда дойдём до конца будет сдвигать элементы а не менять страницу
  menu.setScrollMode(SMENU_SCROLL_ITEM);
  // для под-меню мы можем изменить настройки, к примеру сделать кол-во элементов на странице меньше
  // и сделать скролл страниц
  submenu.init(6);
  submenu.setScrollMode(SMENU_SCROLL_PAGE);
  
  // добавим немного элементов в наше меню
  // можем принимать как с Flash памяти так и с SRAM (надо убрать слово Flash из названии функции)
  // первый аргумент функции - название пункта
  // второй - метод который будет вызван при выборе пункта меню или указатель на под-меню которое надо будет открыть
  
  // в первом меню добавим типо настройки и сделаем пункт меню "игры" кликабельным и чтобы он открывал под-меню
  menu.addFlashMenuItem(F("Wi-Fi"));
  menu.addFlashMenuItem(F("Блютуз"));
  menu.addFlashMenuItem(F("Уведомления"));
  menu.addFlashMenuItem(F("Соц. сети"));
  menu.addFlashMenuItem(F("Навигатор"));
  menu.addFlashMenuItem(F("Калькулятор"));
  menu.addFlashMenuItem(F("Игры"), &submenu);
  menu.addFlashMenuItem(F("Камера"));
  menu.addFlashMenuItem(F("Браузер"));
  menu.addFlashMenuItem(F("Телефон"));
  menu.addFlashMenuItem(F("Ютубчик"));
  // а последней кнопки добавим обработчик по нашей функции
  // она будет вызвана если выбрать этот пункт из меню
  menu.addFlashMenuItem(F("Изменить состояние светодиода на плате"), toggleLED);

  // в под-меню можно добавить кнопку назад, принимает она текст
  submenu.addFlashGoBackMenuItem(F("Назад"));
  submenu.addFlashMenuItem(F("Fluppy Bird"));
  submenu.addFlashMenuItem(F("Google Dino"));
  submenu.addFlashMenuItem(F("Ping-Pong"));
  submenu.addFlashMenuItem(F("Mario"));
  submenu.addFlashMenuItem(F("2048"));
  submenu.addFlashMenuItem(F("Hill Climb Racing"));
  submenu.addFlashMenuItem(F("Tetris"));
  submenu.addFlashMenuItem(F("Это должно быть на второй странице"));
  submenu.addFlashMenuItem(F("Ну допустим ещё какая-то игра"));
  submenu.addFlashMenuItem(F("Калибровка времени"));
  
  // подключаем кнопки к нашему меню, то есть передаём указатели на наши кнопки
  menu.bindEncoder(&enc);
  // и вызываем функцию обновления меню чтобы вывести его
  menu.update();
}

// the loop function runs over and over again until power down or reset
void loop() {
  menu.tick();
}

void toggleLED() {
  // создадим статичную переменную и по умолчанию она будет false
  static boolean led = false;
  // инвертируем значение флажка и записываем его в пин №13
  led = !led;
  digitalWrite(LED_BUILTIN, led);
}
