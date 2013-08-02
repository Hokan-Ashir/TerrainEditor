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

    /**
     * GUI Manager which creates GUI and manage its events calling appropriate functions
     * that's why it must have so "heavy" pointers (to IrrlichtDevice & TerrainEditor classes)
     * 
     * inspired by Yustme thread [http://irrlicht.sourceforge.net/forum/viewtopic.php?t=25972]
     */
    class CGUIManager : public IEventReceiver {
    public:
        /**
         * Constructor
         * 
         * @param pDevice pointer to IrrlichtDevice class instance
         * @param pTerrainEditor        pointer to CTerrainEditor class instance
         */
        CGUIManager(IrrlichtDevice* pDevice, CTerrainEditor* pTerrainEditor);
        
        /**
         * Copy constructor (not implemented)
         * 
         * @param orig reference to original class instance
         */
        CGUIManager(const CGUIManager& orig) {};
        
        /**
         * Virtual destructor (not implemented)
         */
        virtual ~CGUIManager() {};
        
        /**
         * Resets all affected GUI elements (must be called every frame, before any event)
         */
        void resetAllGUIElements();
        
        /**
         * Operates events affecting on GUI - pressing buttons, selecting elements
         * 
         * @param event incomming event
         * @return true if incomming event processed
         */
        virtual bool OnEvent(const SEvent& event);
        
        /**
         * Gets flag of current activated GUI element
         * 
         * @param GUI element type which has been activated
         * @return true if active
         */
	bool getEventCallerByElement(gui::EGUI_EVENT_TYPE);
	
        /**
         * This function will be used to get the ID of a gui element.
         * This is a general function for getting the ID.
         * It works for a lot of gui events but not all.
         * Like getting the ID of the context menu wont work with this function
         * Instead, use this function: getEventCallerOfMenuByID()
         * 
         * @return ID of GUI element
         */
	s32 getEventCallerByID();
        
        /**
         * meant for event: EGET_MENU_ITEM_SELECTED
         * because IGUIContextMenu does not have the function: getID()
         * in this line: event.GUIEvent.Caller->getID()
         * So I had to make a custome one for the EGET_MENU_ITEM_SELECTED events.
         * 
         * @return ID of GUI menu element
         */
	s32 getEventCallerOfMenuByID();        
    private:
        /**
         * Pointer to CTerrainEditor class instance
         */
        CTerrainEditor* pTerrainEditor;
        
        /**
         * Pointer to IrrlichtDevice class instance
         */
        IrrlichtDevice* pDevice;       
        
        /**
         * Array of GUI elements statuses
         */
        bool elementStatus[NUMBER_OF_GUI_ELEMENTS];

        /**
         * Pointer to IGUIContextMenu class instance
         */
	gui::IGUIContextMenu* pGUIContextMenu;
        
        /**
         * Selected menu item ID
         */
	s32 menuItemSelectedID;
        
        /**
         * General selected item ID
         */
	s32 generalCallerID;
    };

} // end of namespace irr

#endif	/* CGUIMANAGER_H */

