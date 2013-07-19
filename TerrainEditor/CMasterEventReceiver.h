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

#ifndef __C_MASTER_EVENT_RECEIVER_H_INCLUDED__
#define __C_MASTER_EVENT_RECEIVER_H_INCLUDED__

#include "IEventReceiver.h"
#include "IGUIContextMenu.h"

#define NUMBER_OF_GUI_ELEMENTS 21

namespace irr
{

/// ==============================
/// MastEventReceiver
/// ==============================
class CMasterEventReceiver : public IEventReceiver
{
// protected;
	// Enumeration for UP, DOWN, PRESSED and RELEASED key states. Also used for mouse button states.
	enum keyStatesENUM {UP, DOWN, PRESSED, RELEASED};

	// Enumeration for Event Handling State.
	enum processStateENUM {STARTED, ENDED};

	// this enum is for setting the gui events to true or false.
	enum ElementStatus
	{
		TRUE,
		FALSE
	};

	// Mouse button states.
	keyStatesENUM mouseButtonState[2]; //Left(0), Middle(1) and Right(2) Buttons.

	// Keyboard key states.
	keyStatesENUM keyState[KEY_KEY_CODES_COUNT];

	// Mouse X/Y coordinates and Wheel data.
	struct mouseData
	{
		u32 X;
		u32 Y;
		f32 wheel; //wheel is how far the wheel has moved
	};

	struct mouseData mouse;

	processStateENUM processState; // STARTED = handling events, ENDED = not handling events

	ElementStatus elementStatus[NUMBER_OF_GUI_ELEMENTS];

	gui::IGUIContextMenu* menu;
	s32 menuItemSelectedID;
	s32 generalCallerID;

	//////////////////////
	// Public functions
	//////////////////////
public:

	CMasterEventReceiver();
	~CMasterEventReceiver();

	virtual bool OnEvent(const SEvent& event);

	f32 getMouseWheel() const;
	u32 getMouseX() const;
	u32 getMouseY() const;

	bool isLeftMouseReleased()const;
	bool isLeftMouseUp() const;
	bool isLeftMousePressed() const;
	bool isLeftMouseDown() const;

	bool isMiddleMouseReleased() const;
	bool isMiddleMouseUp() const;
	bool isMiddleMousePressed() const;
	bool isMiddleMouseDown() const;

	bool isRightMouseReleased() const;
	bool isRightMouseUp() const;
	bool isRightMousePressed() const;
	bool isRightMouseDown() const;

	bool isKeyPressed(char keycode) const;
	bool isKeyDown(char keycode) const;
	bool isKeyUp(char keycode) const;
	bool isKeyReleased(char keycode) const;

	// This is used so that the Key States will not be changed during execution of your Main game loop.
	// Place this at the very START of your Main Loop
	void endEventProcess();

	// This is used so that the Key States will not be changed during execution of your Main game loop.
	// Place this function at the END of your Main Loop.
	void startEventProcess();

	void initialization();

	// GUI events:
	bool getEventCallerByElement(gui::EGUI_EVENT_TYPE);
	
	s32 getEventCallerOfMenuByID();
	s32 getEventCallerByID();

};

}	// end of irr namespace
/// ==========================================
/// END OF MastEventReceiver
/// ==========================================

#endif // end of __C_MASTER_EVENT_RECEIVER_H_INCLUDED__
