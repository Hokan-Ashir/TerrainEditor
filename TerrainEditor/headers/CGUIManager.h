/* 
 * File:   CGUIManager.h
 * Author: hokan
 *
 * Created on 26 Июль 2013 г., 16:22
 */

#ifndef CGUIMANAGER_H
#define	CGUIMANAGER_H

#include <IEventReceiver.h>
#include <IGUIContextMenu.h>
#include <ITerrainSceneNode.h>

#include "CTerrainEditor.h"

#define NUMBER_OF_GUI_ELEMENTS 21

namespace irr {

    class CGUIManager : public IEventReceiver {
    public:
        CGUIManager(IrrlichtDevice* pDevice, CTerrainEditor* pTerrainEditor);
        CGUIManager(const CGUIManager& orig);
        virtual ~CGUIManager();
        virtual bool OnEvent(const SEvent& event);
        void resetAllGUIElements();
        // GUI events:
	bool getEventCallerByElement(gui::EGUI_EVENT_TYPE);
	
	s32 getEventCallerOfMenuByID();
	s32 getEventCallerByID();
    private:
        CTerrainEditor* pTerrainEditor;
        IrrlichtDevice* pDevice;
        
        bool editMode;
        
        enum ElementStatus
	{
		TRUE,
		FALSE
	};
        
        ElementStatus elementStatus[NUMBER_OF_GUI_ELEMENTS];

	gui::IGUIContextMenu* menu;
	s32 menuItemSelectedID;
	s32 generalCallerID;
    };

} // end of namespace irr

#endif	/* CGUIMANAGER_H */

