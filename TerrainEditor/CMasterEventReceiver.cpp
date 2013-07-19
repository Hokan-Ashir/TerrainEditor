/// ==================================================================================================
/// MastEventReceiver code is � (Copyright) Robert E. Demarest, AKA Mastiff or Mastiff Odit
/// This file may be used in any non-commercial or commercial project as long as the following conditions are met:
/// You may not claim this code as being your own.
/// You may not use this code for any harmful, malicious or otherwise damaging programs.
///
/// This is version 1.2a of the class.
/// This class is designed for use with the Irrlicht Engine, it was written for version 1.3 of the engine.
/// ==================================================================================================

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// To use this Class just add  #include "MastEventReceiver.cpp"  to the end of your includes list. (or add the class in-line into your program)
// Then create an instance of it like so: MastEventReceiver eventReceiver;
// Then call the initialization fucntion like so: eventReceiver.init();
// Then inside your Main Game Loop place "eventReceiver.endEventProcess();" in the beginning of your game loop, before anything -
// that would require input, then put "eventReceiver.startEventProcess();" at the very end of your Main Game Loop.
// yeah I know it's confusing, but it makes alot more sense in the internals of the class.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

/**************************************
tbd:
1. GUI-receiver
2. check bugs on forum
3. very expensive ~ 100 Mb
**************************************/

#include "CMasterEventReceiver.h"

namespace irr
{
/// ==============================
/// MasterEventReceiver implementation
/// ==============================
	CMasterEventReceiver::CMasterEventReceiver()
	{
		initialization();
	}

	CMasterEventReceiver::~CMasterEventReceiver()
	{

	}

	bool CMasterEventReceiver::OnEvent(const SEvent& event)
	{
		bool eventprocessed = false;

		//////////////////////////////
		// Keyboard Input Event
		//////////////////////////////
		if (event.EventType == EET_KEY_INPUT_EVENT)
		{
			if (processState == STARTED)
			{
				// if key is Pressed Down
				if (event.KeyInput.PressedDown == true)
				{
					// If key was not down before
					if (keyState[event.KeyInput.Key] != DOWN)
						keyState[event.KeyInput.Key] = PRESSED; // Set to Pressed

					else
						// if key was down before
						keyState[event.KeyInput.Key] = DOWN; // Set to Down
				}
				else
				{
					// if the key is down
					if (keyState[event.KeyInput.Key] != UP)
						keyState[event.KeyInput.Key] = RELEASED; // Set to Released
				}
			}
			eventprocessed = true;
		}

		//////////////////////////////
		// Mouse Input Event
		//////////////////////////////

		if (event.EventType == EET_MOUSE_INPUT_EVENT)
		{
			if (processState == STARTED)
			{
				//Mouse changed position
				if (event.MouseInput.Event == EMIE_MOUSE_MOVED)
				{
					mouse.Y = event.MouseInput.Y;
					mouse.X = event.MouseInput.X;
				}

				//Wheel moved.
				if (event.MouseInput.Event == EMIE_MOUSE_WHEEL)
					mouse.wheel += event.MouseInput.Wheel;

				//Left Mouse Button Pressed
				if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
				{
					//
					if (mouseButtonState[0] == UP || mouseButtonState[0] == RELEASED)
						mouseButtonState[0] = PRESSED;
					else
						mouseButtonState[0] = DOWN;
				}

				//Left Mouse Button Rleased
				if (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
				{
					//
					if (mouseButtonState[0] != UP)
						mouseButtonState[0] = RELEASED;
				}

				//Middle Mouse Button Pressed
				if (event.MouseInput.Event == EMIE_MMOUSE_PRESSED_DOWN)
				{
					//
					if (mouseButtonState[1] == UP || mouseButtonState[1] == RELEASED)
						mouseButtonState[1] = PRESSED;

					else
						mouseButtonState[1] = DOWN;
				}

				//Middle Mouse Button Rleased
				if (event.MouseInput.Event == EMIE_MMOUSE_LEFT_UP)
				{
					//
					if (mouseButtonState[1] != UP)
						mouseButtonState[1] = RELEASED;
				}

				//Right Mouse Button Pressed
				if (event.MouseInput.Event == EMIE_RMOUSE_PRESSED_DOWN)
				{
					//
					if (mouseButtonState[2] == UP || mouseButtonState[2] == RELEASED)
						mouseButtonState[2] = PRESSED;

					else
						mouseButtonState[2] = DOWN;
				}

				//Right Mouse Button Rleased
				if (event.MouseInput.Event == EMIE_RMOUSE_LEFT_UP)
				{
					//
					if (mouseButtonState[2] != UP)
						mouseButtonState[2] = RELEASED;
				}
			}
			eventprocessed = true;
		}

		if (event.EventType == EET_GUI_EVENT)
		{
			generalCallerID = event.GUIEvent.Caller->getID();
			
			switch(event.GUIEvent.EventType)
			{
				case gui::EGET_ELEMENT_FOCUS_LOST:
				case gui::EGET_ELEMENT_FOCUSED:
				case gui::EGET_ELEMENT_HOVERED:
				case gui::EGET_ELEMENT_LEFT:
				case gui::EGET_ELEMENT_CLOSED:
				case gui::EGET_BUTTON_CLICKED:
				case gui::EGET_SCROLL_BAR_CHANGED:
				case gui::EGET_CHECKBOX_CHANGED:
				case gui::EGET_LISTBOX_CHANGED:
				case gui::EGET_LISTBOX_SELECTED_AGAIN:
				case gui::EGET_FILE_SELECTED:
				case gui::EGET_FILE_CHOOSE_DIALOG_CANCELLED:
				case gui::EGET_MESSAGEBOX_YES:
				case gui::EGET_MESSAGEBOX_NO:
				case gui::EGET_MESSAGEBOX_OK:
				case gui::EGET_MESSAGEBOX_CANCEL:
				case gui::EGET_EDITBOX_ENTER:
				case gui::EGET_TAB_CHANGED:
				case gui::EGET_COMBO_BOX_CHANGED:
				case gui::EGET_SPINBOX_CHANGED:
					elementStatus[event.GUIEvent.EventType] = TRUE;
				
				break;

				case gui::EGET_MENU_ITEM_SELECTED:
					elementStatus[gui::EGET_MENU_ITEM_SELECTED] = TRUE;
					menu = (gui::IGUIContextMenu*)event.GUIEvent.Caller; 
					menuItemSelectedID = menu->getItemCommandId(menu->getSelectedItem());
				break;
			}
		}

		// or we can't pass events to other user-control animators or event-receivers
		return false;
	}


	f32 CMasterEventReceiver::getMouseWheel() const
	{
		return mouse.wheel;
	}

	u32 CMasterEventReceiver::getMouseX() const
	{
		return mouse.X;
	}

	u32 CMasterEventReceiver::getMouseY() const
	{
		return mouse.Y;
	}

	bool CMasterEventReceiver::isLeftMouseReleased() const
	{
		if (mouseButtonState[0] == RELEASED)
			return true;

		else
			return false;
	}

	bool CMasterEventReceiver::isLeftMouseUp() const
	{
		if (mouseButtonState[0] == RELEASED || mouseButtonState[0] == UP)
			return true;

		else
			return false;
	}

	bool CMasterEventReceiver::isLeftMousePressed() const
	{
		if (mouseButtonState[0] == PRESSED)
			return true;

		else
			return false;
	}

	bool CMasterEventReceiver::isLeftMouseDown() const
	{
		if (mouseButtonState[0] == PRESSED || mouseButtonState[0] == DOWN)
			return true;

		else
			return false;
	}

	bool CMasterEventReceiver::isMiddleMouseReleased() const
	{
		if (mouseButtonState[1] == RELEASED)
			return true;

		else
			return false;
	}

	bool CMasterEventReceiver::isMiddleMouseUp() const
	{
		if (mouseButtonState[1] == RELEASED || mouseButtonState[1] == UP)
			return true;

		else
			return false;
	}

	bool CMasterEventReceiver::isMiddleMousePressed() const
	{
		if (mouseButtonState[1] == PRESSED)
			return true;

		else
			return false;
	}

	bool CMasterEventReceiver::isMiddleMouseDown() const
	{
		if (mouseButtonState[1] == PRESSED || mouseButtonState[1] == DOWN)
			return true;

		else
			return false;
	}

	bool CMasterEventReceiver::isRightMouseReleased() const
	{
		if (mouseButtonState[2] == RELEASED)
			return true;

		else
			return false;
	}

	bool CMasterEventReceiver::isRightMouseUp() const
	{
		if (mouseButtonState[2] == RELEASED || mouseButtonState[2] == UP)
			return true;

		else
			return false;
	}

	bool CMasterEventReceiver::isRightMousePressed() const
	{
		if (mouseButtonState[2] == PRESSED)
			return true;

		else
			return false;
	}

	bool CMasterEventReceiver::isRightMouseDown() const
	{
		if (mouseButtonState[2] == PRESSED || mouseButtonState[2] == DOWN)
			return true;

		else
			return false;
	}//

	bool CMasterEventReceiver::isKeyPressed(char keycode) const
	{
		if (keyState[keycode] == PRESSED)
			return true;

		else
			return false;
	}

	bool CMasterEventReceiver::isKeyDown(char keycode) const
	{
		if (keyState[keycode] == DOWN || keyState[keycode] == PRESSED)
			return true;

		else
			return false;
	}

	bool CMasterEventReceiver::isKeyUp(char keycode) const
	{
		if (keyState[keycode] == UP || keyState[keycode] == RELEASED)
			return true;

		else
			return false;
	}

	bool CMasterEventReceiver::isKeyReleased(char keycode) const
	{
		if (keyState[keycode] == RELEASED)
			return true;

		else
			return false;
	}

	bool CMasterEventReceiver::getEventCallerByElement(gui::EGUI_EVENT_TYPE guiEventType)
	{
		if(elementStatus[guiEventType] == TRUE)
			return true;

		return false;
		//return elementStatus[guiEventType];
	}

	// This function will be used to get the ID of a gui element.
	// This is a general function for getting the ID.
	// It works for a lot of gui events but not all.
	// Like getting the ID of the context menu wont work with this function
	// Instead, use this function: getEventCallerOfMenuByID()
	s32 CMasterEventReceiver::getEventCallerByID()
	{
		return generalCallerID;
	}

	// meant for event: EGET_MENU_ITEM_SELECTED
	// because IGUIContextMenu does not have the function: getID()
	// in this line: event.GUIEvent.Caller->getID()
	// So I had to make a custome one for the EGET_MENU_ITEM_SELECTED events.
	s32 CMasterEventReceiver::getEventCallerOfMenuByID()
	{
		return menuItemSelectedID;
	}

	// This is used so that the Key States will not be changed during execution of your Main game loop.
	// Place this at the very START of your Main Loop
	void CMasterEventReceiver::endEventProcess()
	{
		processState = ENDED;
	}

	// This is used so that the Key States will not be changed during execution of your Main game loop.
	// Place this function at the END of your Main Loop.
	void CMasterEventReceiver::startEventProcess()
	{
		processState = STARTED;
		//Keyboard Key States
		for (int i = 0; i < KEY_KEY_CODES_COUNT; i++)
		{
			if (keyState[i] == RELEASED)
				keyState[i] = UP;

			if (keyState[i] == PRESSED)
				keyState[i] = DOWN;
		}
		//Mouse Button States
		for (int i = 0; i <= 2; i++)
		{
			if (mouseButtonState[i] == RELEASED)
				mouseButtonState[i] = UP;

			if (mouseButtonState[i] == PRESSED)
				mouseButtonState[i] = DOWN;
		}

		//Mouse Wheel state
		mouse.wheel = 0.0f;

		// Set all gui elements to false
		elementStatus[gui::EGET_ELEMENT_FOCUS_LOST] = elementStatus[gui::EGET_ELEMENT_FOCUSED] = 
		elementStatus[gui::EGET_ELEMENT_HOVERED] = elementStatus[gui::EGET_ELEMENT_LEFT] =
		elementStatus[gui::EGET_ELEMENT_CLOSED] = elementStatus[gui::EGET_BUTTON_CLICKED] = 
		elementStatus[gui::EGET_SCROLL_BAR_CHANGED] = elementStatus[gui::EGET_CHECKBOX_CHANGED] =
		elementStatus[gui::EGET_LISTBOX_CHANGED] = elementStatus[gui::EGET_LISTBOX_SELECTED_AGAIN] = 
		elementStatus[gui::EGET_FILE_SELECTED] = elementStatus[gui::EGET_FILE_CHOOSE_DIALOG_CANCELLED] =
		elementStatus[gui::EGET_MESSAGEBOX_YES] = elementStatus[gui::EGET_MESSAGEBOX_NO] = 
		elementStatus[gui::EGET_MESSAGEBOX_OK] = elementStatus[gui::EGET_MESSAGEBOX_CANCEL] = 
		elementStatus[gui::EGET_EDITBOX_ENTER] = elementStatus[gui::EGET_TAB_CHANGED] = 
		elementStatus[gui::EGET_MENU_ITEM_SELECTED] = elementStatus[gui::EGET_COMBO_BOX_CHANGED] = 
		elementStatus[gui::EGET_SPINBOX_CHANGED] = FALSE; 
	}

	void CMasterEventReceiver::initialization()
	{
		//KeyBoard States.
		for (int i = 0; i <= KEY_KEY_CODES_COUNT; i++)
			keyState[i] = UP;

		//Mouse states
		for (int i = 0; i <= 2; i++)
			mouseButtonState[i] = UP;

		//Mouse X/Y coordenates.
		mouse.X = 0;
		mouse.Y = 0;
		mouse.wheel = 0.0f;
	}

} // end of namespace irr
