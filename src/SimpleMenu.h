// second-include prevention & define for other libraries
#ifndef _simpleMenu_lib_H
#define _simpleMenu_lib_H

#pragma pack(push, 1)

// some checkers for libraries support
// (e.g include OLED properties based on the GyverOLED library)
#pragma region GyverLibs checkers

// check if GyverOLED library is loaded
// since there is no direct define of the library
// we will check for the buffer and oled sizes types
#pragma region GyverOLED checker
#if ((defined(OLED_HARD_BUFFER_64) && defined(HARD_BUFFER)) || (defined(OLED_SOFT_BUFFER_32) && defined(SOFT_BUFFER)) || defined(OLED_NO_BUFFER)) && defined(OLED128x32) && defined(OLED128x64)
#define _OLED_SUPPORT
#endif
#pragma endregion

// check if GyverButton is loaded
// since there is no direct define of the library
// we will check for some defines (probably they will not be used in other libraries  
#pragma region GyverButton checker
#if defined(_buttonRead) && BTN_NO_PIN == -1 && NORM_OPEN == 0 && NORM_CLOSE == 1
#define _BUTTON_SUPPORT
#endif  
#pragma endregion

// check if GyverEncoder is loaded
// since there is no direct define of the library
// we will check for some defines (probably they will not be used in other libraries  
#pragma region GyverEncoder checker
#if ENC_NO_BUTTON == -1 && defined(ENC_WITH_BUTTON) && defined(DEFAULT_ENC_PULL) && defined(ENC_DEBOUNCE_TURN) && defined(ENC_DEBOUNCE_BUTTON)
#define _ENCODER_SUPPORT
#endif
#pragma endregion


#pragma endregion

// check if arduino loaded and define some checks based on the load-state
// of this base library, for example, based on one of this macros, supporting
// for flash memory converters can be disabled/enabled, based on the arduino
// core, since the flash and PROGMEM defined in this core
#pragma region Arduino Core Loader Verification
#ifdef Arduino_h
#define _sys1_macro_arduino_loaded
#else
#define _sys1_macro_arduino_loaded && false
#endif  
#pragma endregion

// define some I/O defines for modes
#pragma region I/O defines
#define SMENU_INPUT_MANUAL		(0)
#define SMENU_INPUT_SUBMENU		SMENU_INPUT_MANUAL
#define SMENU_INPUT_BUTTON		(1)
#define SMENU_INPUT_ENCODER		(2)

#define SMENU_OUTPUT_MANUAL		(0)
#define SMENU_OUTPUT_SUBMENU	SMENU_OUTPUT_MANUAL
#define SMENU_OUTPUT_OLED		(1)
#pragma endregion
// define some types of scrolling (e.g by item or by page)
#pragma region Scrolling types defines
#define SMENU_SCROLL_ITEM		(0)
#define SMENU_SCROLL_PAGE		(1)
#pragma endregion

// based on the defines, create an alias for base-type
// and call it with one name to use it later anywhere
// you need for
#pragma region SMenuItemNum typedef
#if !defined(SMENU_ITEMBUFSIZE_2) && !defined(SMENU_ITEMBUFSIZE_4)
typedef unsigned char SMenuItemNum;
#elif defined(SMENU_ITEMBUFSIZE_2)
typedef unsigned short SMenuItem;
#elif defined(SMENU_ITEMBUFSIZE_4)
typedef unsigned long SMenuItem;
#endif  
#pragma endregion

// some defines to make the working with buttons a lot easier
#ifdef _BUTTON_SUPPORT
#define SMENU_BTN_DOWN	(0)
#define SMENU_BTN_UP	(1)
#define SMENU_BTN_SEL	(2)
#endif


// struct contains the response to handle manual render of items
struct SMenuRenderItems {
	SMenuItemNum count = 0;
	char** items;
};

// base class of the SimpleMenu
class SimpleMenu {

#pragma region private type aliases & structures
private: // type aliases & structures
// on-click callback handler type
	typedef void(*MENUITEM_ONCLICK)();

	// menu-item structure
	struct MenuItem {
		union {
			struct {
				unsigned char
					arrayInitialized : 1,
					menuGoBackItem : 1,
					isFlashMemory : 1;
			};
			unsigned char flags;
		};

		MenuItem(const char* text)
		{
			this->text = (void*)text;
			flags = 0;
		};
		MenuItem(const char* text, bool arrayInitialized)
		{
			this->text = (void*)text;
			flags = 0;
			this->arrayInitialized = arrayInitialized ? 1 : 0;
		};
#ifdef Arduino_h
		MenuItem(const __FlashStringHelper* text)
		{
			this->text = (void*)text;
			flags = 0;
			this->isFlashMemory = 1;
		}
		MenuItem(const __FlashStringHelper* text, bool arrayInitialized)
		{
			this->text = (void*)text;
			flags = 0;
			this->arrayInitialized = arrayInitialized ? 1 : 0;
			this->isFlashMemory = 1;
		};
#endif

		~MenuItem()
		{
			if (arrayInitialized)
				delete[] text;
			if (subMenu)
				delete subMenu;
		}

		void* text = 0;
		SimpleMenu* subMenu = 0;
		MENUITEM_ONCLICK onItemClick = 0;
	};
#pragma endregion

#pragma region private methods
private: // methods
	inline MenuItem* _AddMenuItem(const char item[])
	{
		/*size_t bufSize = strlen(item) + 1;
		char* buf = new char[bufSize];
		strcpy(buf, item);
		buf[bufSize - 1] = '\0';*/
		MenuItem* newItem = IncreaseMenuItemsSize()[0] = new MenuItem(item);
		return newItem;
	}
#ifdef Arduino_h
	inline MenuItem* _AddMenuItem(const __FlashStringHelper* item)
	{
		MenuItem* newItem = IncreaseMenuItemsSize()[0] = new MenuItem(item);
		return newItem;
	}
#endif

	MenuItem** IncreaseMenuItemsSize(SMenuItemNum count = 1)
	{
		// create a new array with more items
		MenuItem** newTmp = new MenuItem * [this->_menuItemsCount + count];
		// cycle over the original array and copy all it's elements
		for (SMenuItemNum i = 0; i < this->_menuItemsCount; i++)
			newTmp[i] = this->_menuItems[i];
		// delete from memory the original array
		delete[] this->_menuItems;
		// swap the pointer of the original with the new one
		this->_menuItems = newTmp;
		// return a pointer to the array with zero-index of the new items
		return newTmp + this->_menuItemsCount++;
	}
#pragma endregion

#pragma region private properties
private: // properties
	unsigned char _inputDevice;
	unsigned char _outputDevice;

	MenuItem** _menuItems;
	SMenuItemNum _menuItemsCount = 0;

	SimpleMenu* openedBy = 0;
	SimpleMenu* openedSubMenu = 0;

	struct {
		unsigned char size;
		unsigned char cursorPos;
		union {
			SMenuItemNum firstItemOnList;
			unsigned char page;
		};
		unsigned char scrollingType = SMENU_SCROLL_ITEM;
	} pageProps;

	#pragma region Output device pointer
	union {

		// a pointer to function to call for manual render
		void(*manual)(SMenuRenderItems);

		#pragma region GyverOLED
#ifdef _OLED_SUPPORT
		GyverOLED* oled;
#endif  
#pragma endregion

	} outputDevices;
#pragma endregion

	#pragma region Input device pointer
	union _inputDevices {
		_inputDevices(){}

		#pragma region GyverButton
#ifdef _BUTTON_SUPPORT

		struct {
			GButton* down = 0;
			GButton* up = 0;
			GButton* select = 0;
		} buttons;

#endif
#pragma endregion

		#pragma region GyverEncoder
#ifdef _ENCODER_SUPPORT
		Encoder* encoder;
#endif
		#pragma endregion


	} inputDevices;
#pragma endregion


#pragma endregion

#pragma region public methods
public: // methods
	#pragma region Constructors & Deconstructor
			// constructor
	SimpleMenu(byte inputDevice, byte outputDevice) : _inputDevice(inputDevice), _outputDevice(outputDevice)
	{
		this->_menuItems = new MenuItem * [0];
	}
	// deconstructor
	~SimpleMenu()
	{
		// cycle over all menu items
		for (SMenuItemNum i = 0; i < sizeof(this->_menuItems); i++)
			// and try delete their text from the RAM
			if (this->_menuItems[i] != 0) delete this->_menuItems[i];
		// after removing all menu items, we can delete also the space for the menu items array
		delete[] this->_menuItems;
	}
#pragma endregion

	#pragma region Item managment methods
	// add a simple menu item without any action
	void addMenuItem(const char item[])
	{
		_AddMenuItem(item);
	}
	// add a menu item with "on click" callback
	void addMenuItem(const char item[], MENUITEM_ONCLICK onClick)
	{
		_AddMenuItem(item)->onItemClick = onClick;
	}
	// add a menu item with sub-menu item
	void addMenuItem(const char item[], SimpleMenu* subMenu)
	{
		_AddMenuItem(item)->subMenu = subMenu;
	}
#pragma region Flash and PROGMEM methods based on Arduino Core load state
#ifdef Arduino_h
	// add a simple menu item without any action
	void addFlashMenuItem(const __FlashStringHelper* item)
	{
		_AddMenuItem(item);
	}
	// add a menu item with "on click" callback
	void addFlashMenuItem(const __FlashStringHelper* item, MENUITEM_ONCLICK onClick)
	{
		_AddMenuItem(item)->onItemClick = onClick;
	}
	// add a menu item with sub-menu item
	void addFlashMenuItem(const __FlashStringHelper* item, SimpleMenu* subMenu)
	{
		_AddMenuItem(item)->subMenu = subMenu;
	}
#endif  
#pragma endregion

	// add a menu item which will go up to the parent menu
	inline void addGoBackMenuItem(const char item[])
	{
		_AddMenuItem(item)->menuGoBackItem = true;
	}
#pragma region Flash and PROGMEM methods based on Arduino Core load state
#ifdef Arduino_h
	// add a menu item which will go up to the parent menu
	inline void addFlashGoBackMenuItem(const __FlashStringHelper* item)
	{
		_AddMenuItem(item)->menuGoBackItem = true;
	}
#endif  
#pragma endregion  
#pragma endregion

	#pragma region Control Methods
	// emulate a down button click
	void clickDown()
	{
		// check if we do have a sub-menu opened
		// since all the events we are going to pass
		// to the sub-menus instead of checking all the buttons
		// on all the sub-menus
		if (openedSubMenu)
		{
			openedSubMenu->clickDown();
			goto RET;
		}

		switch (pageProps.scrollingType)
		{
			case SMENU_SCROLL_ITEM:
				// add 1 to cursor position
				pageProps.cursorPos++;
				// check if the cursor position exceeds the page size (pageSize - 1 since the first item is on position 0)
				if (pageProps.cursorPos >= pageProps.size)
				{
					// if so decrease 1 from the cursor position
					pageProps.cursorPos--;

					// increase the index of the first item by 1
					pageProps.firstItemOnList++;
					// check if the first item index is too big and it's not exist
					if (pageProps.firstItemOnList >= _menuItemsCount)
						// and decrease by 1
						pageProps.firstItemOnList--;
					// also check if the item the cursor pointing for is exist, and if not decrease the first item by 1
					else if (pageProps.cursorPos + pageProps.firstItemOnList >= _menuItemsCount)
						// and decrease the first index by 1
						pageProps.firstItemOnList--;
					// probably done.
				}
				// if not check if the item the cursor is pointing to not exist
				else if (pageProps.cursorPos + pageProps.firstItemOnList >= _menuItemsCount)
					// and decrease the cursor position by 1
					pageProps.cursorPos--;
				break;
			case SMENU_SCROLL_PAGE:
				// add 1 to cursor position
				pageProps.cursorPos++;
				// check if the cursor position exceeds the page size (pageSize - 1 since the first item is on position 0)
				if (pageProps.cursorPos >= pageProps.size)
				{
					// if so check if there is other pages to output
					if (++pageProps.page * pageProps.size < _menuItemsCount)
					{
						// reset the cursor position back to start
						pageProps.cursorPos = 0;
					}
					// and if not decrease the current page and leave it on
					else pageProps.page--;
				}
				// otherwise, if the cursor do have space to move buttom
				// check if there are any items to move to them
				// and if not decrease the cursor position back to original and move on
				if (pageProps.cursorPos + pageProps.page * pageProps.size >= _menuItemsCount)
					// set the cursor position back to original, since we added 1 to the position
					// decrease 1 from it
					pageProps.cursorPos--;

				break;
		}

	RET:
		// check if this menu is opened by someone
		// and if not call the updater
		if (!openedBy) update();
	}
	// emulate an up button click
	void clickUp()
	{
		// check if we do have a sub-menu opened
		// since all the events we are going to pass
		// to the sub-menus instead of checking all the buttons
		// on all the sub-menus
		if (openedSubMenu)
		{
			openedSubMenu->clickUp();
			goto RET;
		}

		switch (pageProps.scrollingType)
		{
			case SMENU_SCROLL_ITEM:
				// first of all check if the current cursor position is already 0 (at the start of the page)
				if (pageProps.cursorPos == 0)
				{
					// and if so we can't move the cursor position up
					// instead we need to change the first element
					// but for this let's check if we do have items above the first one we already rendering
					if (pageProps.firstItemOnList != 0)
						// and if so we can decrease the first item to render
						pageProps.firstItemOnList--;
				}
				// but if we aren't at the start of the page
				// we can decrease the cursor position
				else pageProps.cursorPos--;
				break;
			case SMENU_SCROLL_PAGE:
				// first of all check if the current cursor position is already 0 (at the start of the page)
				if (pageProps.cursorPos == 0)
				{
					// check if we aren't at the first page already
					if (pageProps.page != 0)
					{
						// move 1 page up
						pageProps.page--;
						// and set the cursor to the last element on the page
						pageProps.cursorPos = pageProps.size - 1;
					}
				}
				// but if we aren't at the start of the page
				// we can decrease the cursor position
				else pageProps.cursorPos--;
				break;
			default:
				break;
		}

	RET:
		// check if this menu is opened by someone
		// and if not call the updater
		if (!openedBy) update();
	}
	// emulate a select button click (AKA ok button)
	void clickSelect()
	{
		// check if we do have a sub-menu opened
		// since all the events we are going to pass
		// to the sub-menus instead of checking all the buttons
		// on all the sub-menus
		if (openedSubMenu) openedSubMenu->clickSelect();
		else
		{
			// calculate the selected item index
			SMenuItemNum selectedItemIndex;
			switch (pageProps.scrollingType)
			{
				case SMENU_SCROLL_ITEM:
					selectedItemIndex = pageProps.firstItemOnList + pageProps.cursorPos;
					break;
				case SMENU_SCROLL_PAGE:
					selectedItemIndex = pageProps.page * pageProps.size + pageProps.cursorPos;
					break;
			}
			// store the pointer to the selected item
			MenuItem* selectedItem = _menuItems[selectedItemIndex];


			// if the item suppose to move to the previous menu
			// sub-menu  ->  original menu
			if (selectedItem->menuGoBackItem)
			{
				openedBy->openedSubMenu = 0;
				openedBy = 0;
			}
			// if the item suppose to open a sub-menu
			else if (selectedItem->subMenu)
			{
				selectedItem->subMenu->openedBy = this;
				openedSubMenu = selectedItem->subMenu;
			}
			// but if there is no special action to the menu
			// we just process the onclick action
			else if (selectedItem->onItemClick) selectedItem->onItemClick();
		}

		// check if this menu is opened by someone
		// and if not call the updater
		if (!openedBy) update();
	}

	// initialize the menu
	inline void init(SMenuItemNum pageSize)
	{
		pageProps.size = pageSize;
	}

	// set the scrolling mode of this menu (see scrolling mode defines)
	inline void setScrollMode(unsigned char mode)
	{
		pageProps.scrollingType = mode;
	}

	// updates the menu
	inline void update()
	{
		switch (_outputDevice)
		{
			case SMENU_OUTPUT_MANUAL:
				if (outputDevices.manual)
				{
					SMenuRenderItems rendItems = getRenderMenuItems();
					outputDevices.manual(rendItems);
					clearRenderMenuItems(rendItems);
				}
				break;
#ifdef _OLED_SUPPORT
			case SMENU_OUTPUT_OLED:
				renderOLED();
				break;
#endif
		}
	}

	// make the menu check for inputs and update the menu if needed
	void tick()
	{
		switch (_inputDevice)
		{
#ifdef _BUTTON_SUPPORT
			case SMENU_INPUT_BUTTON:
				if (inputDevices.buttons.down)
				{
					inputDevices.buttons.down->tick();
					if (inputDevices.buttons.down->isClick() || inputDevices.buttons.down->isHold())
					{
						clickDown();
						return;
					}
				}
				if (inputDevices.buttons.up)
				{
					inputDevices.buttons.up->tick();
					if (inputDevices.buttons.up->isClick() || inputDevices.buttons.up->isHold())
					{
						clickUp();
						return;
					}
				}
				if (inputDevices.buttons.select)
				{
					inputDevices.buttons.select->tick();
					if (inputDevices.buttons.select->isClick() || inputDevices.buttons.select->isHold())
					{
						clickSelect();
						return;
					}
				}
				break;
#endif
#ifdef _ENCODER_SUPPORT
			case SMENU_INPUT_ENCODER:
				// check if there is a pointer to the encoder and it's got turned
				if (inputDevices.encoder)
				{
					inputDevices.encoder->tick();
					if (inputDevices.encoder->isTurn())
					{
						// check if we are trying to scroll right (to the next menu)
						if (inputDevices.encoder->isRight())
							clickDown();
						// check to the other side
						else if (inputDevices.encoder->isLeft())
							clickUp();
						// and check also for a click
						else if (inputDevices.encoder->isClick())
							clickSelect();
					}
				}
				break;
#endif
			default:
				break;
		}
	}
#pragma endregion

	#pragma region Render menu items methods
	SMenuRenderItems getRenderMenuItems()
	{
		// first of all check if the menu opened a sub-menu
		if (openedSubMenu)
			// and if so return the render menu items of it
			return openedSubMenu->getRenderMenuItems();

		SMenuRenderItems items;

		// initialize an array for all the items (maximum count of items is the page size)
		items.items = new char* [pageProps.size];

		// calcualte the first element to render
		SMenuItemNum firstElementToRender;
		switch (pageProps.scrollingType)
		{
			case SMENU_SCROLL_ITEM:
				firstElementToRender = pageProps.firstItemOnList;
				break;
			case SMENU_SCROLL_PAGE:
			default:
				firstElementToRender = pageProps.page * pageProps.size;
				break;
		}

		// cycle over page-size of items from the first item to render
		for (SMenuItemNum i = firstElementToRender; i < firstElementToRender + pageProps.size; i++)
		{
			// check if there are any free items to render
			if (i >= _menuItemsCount)
				// exit the loop if there are no free items to render
				break;

			// create a buffer for the menu item
			// +2 since we may have the pointer for the row (if the item is currently selected) and space between this pointer (or alignment to other items if there is no pointer on this row)
			// +1 for string terminating character
			size_t bufSize = 3;
			if (_menuItems[i]->isFlashMemory _sys1_macro_arduino_loaded) // if we are working with flash memory
			{
#ifdef Arduino_h
				// loop until we are gettting a terminating string character (\0)
				while (pgm_read_byte(_menuItems[i]->text + (bufSize - 3)))
				{
					// and increase the buffer size
					bufSize++;
				}
#endif
			}
			else bufSize += strlen((const char*)_menuItems[i]->text); // but if we are working with plain char array, we can just calculate the length using strlen function
			char* buf = new char[bufSize];

			// check if the current selected item is the item we are currently rendering, and if so insert the correspond character
			// but if not, insert a space
			if (i == pageProps.cursorPos + firstElementToRender) buf[0] = '>';
			else buf[0] = ' ';
			// insert a space, 'cuz why not
			buf[1] = ' ';
			// copy the menu item text to the buffer
			if (_menuItems[i]->isFlashMemory _sys1_macro_arduino_loaded) // if we are working with flash memory
			{
#ifdef Arduino_h
				// cycle until we are getting to the end of the buffer
				// but since the first characters of the buffer is reserved
				// we are starting from index 2, we will decrease the y value when
				// we will read the byte from the flash memory
				for (size_t y = 2; y < bufSize; y++)
				{
					// store the byte of the flash memory in the dynamic memory
					buf[y] = pgm_read_byte(_menuItems[i]->text + (y - 2));
				}
#endif
			}
			else strcpy(buf + 2, (const char*)_menuItems[i]->text);
			// insert a line ending character to the buffer
			buf[bufSize - 1] = '\0';

			// set the pointer to the buffer in the items array
			items.items[items.count++] = buf;
		}

		// return the items
		return items;
	}

	void clearRenderMenuItems(SMenuRenderItems items)
	{
		for (size_t i = 0; i < items.count; i++)
		{
			delete[] items.items[i];
		}
		delete[] items.items;
	}
#pragma endregion

	#pragma region Methods for OLED, will be defined only if OLED library is loaded
#ifdef _OLED_SUPPORT
	// initialize the menu to use with OLED display
	void initOLED(unsigned char type, GyverOLED* oled)
	{
		outputDevices.oled = oled;
		switch (type)
		{
			case OLED128x32:
			case OLED128x64:
				pageProps.size = (type + 1) * 4;
				break;
		}
	}

	// renderes a menu on the OLED display
	void renderOLED()
	{
		// clear the whole display
		outputDevices.oled->clear();
		// move the cursor to home
		outputDevices.oled->home();
		// get the items to render
		SMenuRenderItems items = getRenderMenuItems();
		for (size_t i = 0; i < items.count; i++)
		{
			outputDevices.oled->print(items.items[i]);
			outputDevices.oled->printChar('\n');
		}
		clearRenderMenuItems(items);
		// done!
	}
#endif  
#pragma endregion

	#pragma region Methods which require GyverButton library
#ifdef _BUTTON_SUPPORT
	
	inline void bindButton(GButton* button, byte action)
	{
		switch (action)
		{
			case SMENU_BTN_DOWN:
				inputDevices.buttons.down = button;
				break;
			case SMENU_BTN_UP:
				inputDevices.buttons.up = button;
				break;
			case SMENU_BTN_SEL:
				inputDevices.buttons.select = button;
				break;
		}
	}
	
	inline void bindButton(GButton* down, GButton* up, GButton* select)
	{
		inputDevices.buttons.down = down;
		inputDevices.buttons.up = up;
		inputDevices.buttons.select = select;
	}

#endif  
#pragma endregion

	#pragma region Methods which require GyverEncoder library
#ifdef _ENCODER_SUPPORT

	inline void bindEncoder(Encoder* encoder)
	{
		inputDevices.encoder = encoder;
	}

#endif
#pragma endregion

	#pragma region Manual Output Binder

	void bindOutputFunction(void(*function)(SMenuRenderItems))
	{
		outputDevices.manual = function;
	}

#pragma endregion


#pragma endregion

};

#pragma region Undefine support defines of the library

#ifdef _OLED_SUPPORT
#undef _OLED_SUPPORT
#endif
#ifdef _BUTTON_SUPPORT
#undef _BUTTON_SUPPORT
#endif
#ifdef _ENCODER_SUPPORT
#undef _ENCODER_SUPPORT
#endif

#pragma endregion


#endif