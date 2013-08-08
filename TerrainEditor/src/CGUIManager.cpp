/* 
 * File:   CGUIEventManager.cpp
 * Author: hokan
 * 
 * Created on 26 Июль 2013 г., 16:22
 */

#include "../headers/CGUIManager.h"
#include "../headers/CGUIMainMenu.h"
#include "../headers/terrain.editor/CSceneNodeAnimatorCameraTerrain.h"

namespace irr {

    /**
     * Constructor
     * 
     * @param pDevice pointer to IrrlichtDevice class instance
     * @param pTerrainEditor        pointer to CTerrainEditor class instance
     */
    CGUIManager::CGUIManager(IrrlichtDevice* pDevice, CTerrainEditor* pTerrainEditor) {
        this->pDevice = pDevice;
        this->pTerrainEditor = pTerrainEditor;

        // creates GUI itself
        gui::CGUIMainMenu mainMenu = gui::CGUIMainMenu(pDevice->getGUIEnvironment());
        
        /*gui::IGUISkin* defaultSkin = pDevice->getGUIEnvironment()->createSkin(gui::EGST_BURNING_SKIN);
	pDevice->getGUIEnvironment()->setSkin(defaultSkin);
	defaultSkin->drop();*/
    }

    /**
     * Resets all affected GUI elements (must be called every frame, before any event)
     */
    void CGUIManager::resetAllGUIElements() {
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
                elementStatus[gui::EGET_SPINBOX_CHANGED] = false;
    }

    /**
     * Operates events affecting on GUI - pressing buttons, selecting elements
     * 
     * @param event incomming event
     * @return true if incomming event processed
     */
    bool CGUIManager::OnEvent(const SEvent& event) {
        // parsing GUI event
        if (event.EventType == EET_GUI_EVENT) {
            generalCallerID = event.GUIEvent.Caller->getID();

            switch (event.GUIEvent.EventType) {
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
                    elementStatus[event.GUIEvent.EventType] = true;

                    break;

                case gui::EGET_MENU_ITEM_SELECTED:
                    elementStatus[gui::EGET_MENU_ITEM_SELECTED] = true;
                    pGUIContextMenu = (gui::IGUIContextMenu*)event.GUIEvent.Caller;
                    menuItemSelectedID = pGUIContextMenu->getItemCommandId(pGUIContextMenu->getSelectedItem());
                    break;
            }

            if (/*pDevice->getGUIEnvironment()->hasFocus(pDevice->getGUIEnvironment()->getRootGUIElement())
                    && */this->getEventCallerByID() == gui::GUI_ID_BUTTON_SAVE_SCENE
                    && this->getEventCallerByElement(gui::EGET_BUTTON_CLICKED)) {
                // pTerrainEditor->getCameraSceneNode()->setInputReceiverEnabled(false);
                // create the toolbox window
                gui::IGUIWindow* saveTerrainHeightMapWindow = pDevice->getGUIEnvironment()->addWindow(core::rect<s32>(
                        core::position2d<s32>(pDevice->getVideoDriver()->getScreenSize().Width / 2 - 150,
                        pDevice->getVideoDriver()->getScreenSize().Height / 2 - 100),
                        core::position2d<s32>(pDevice->getVideoDriver()->getScreenSize().Width / 2 + 150,
                        pDevice->getVideoDriver()->getScreenSize().Height / 2 + 100)),
                        false, L"Save terrain heightmap", 0, gui::GUI_ID_WINDOW_SAVE_HEIGHTMAP);

                pDevice->getGUIEnvironment()->addStaticText(L"Enter terrain heightmap file name:", core::rect<s32>(10, 48, 190, 66), false, false, saveTerrainHeightMapWindow);
                pDevice->getGUIEnvironment()->addEditBox(L"", core::rect<s32>(10, 66, 190, 88), true, saveTerrainHeightMapWindow, gui::GUI_ID_EDITBOX_HEIGHTMAP_NAME);
                // save current terrain heightmap
                // pTerrainEditor->saveTerrainHeightMap("heightmap_");
                // pTerrainEditor->getCameraSceneNode()->setInputReceiverEnabled(true);
            } else if (/*pDevice->getGUIEnvironment()->hasFocus(pDevice->getGUIEnvironment()->getRootGUIElement())
                    && */this->getEventCallerByID() == gui::GUI_ID_BUTTON_PAINT
                    && this->getEventCallerByElement(gui::EGET_BUTTON_CLICKED)) {
                // Turn on/off painting-edit mode
                // & turn off lifting-edit mode if active
                pTerrainEditor->setPaintingEditMode(!pTerrainEditor->getPaintingEditMode());
                if (pTerrainEditor->getLiftingEditMode()) {
                    pTerrainEditor->setLiftingEditMode(!pTerrainEditor->getLiftingEditMode());
                }
            } else if (/*pDevice->getGUIEnvironment()->hasFocus(pDevice->getGUIEnvironment()->getRootGUIElement())
                    && */this->getEventCallerByID() == gui::GUI_ID_BUTTON_LIFT_DOWN
                    && this->getEventCallerByElement(gui::EGET_BUTTON_CLICKED)) {
                // Turn on/off lifting-edit mode
                // & turn off painting-edit mode if active
                pTerrainEditor->setLiftingEditMode(!pTerrainEditor->getLiftingEditMode());
                if (pTerrainEditor->getPaintingEditMode()) {
                    pTerrainEditor->setPaintingEditMode(!pTerrainEditor->getPaintingEditMode());
                }
            }
        }
        return false;
    }

    /**
     * Gets flag of current activated GUI element
     * 
     * @param GUI element type which has been activated
     * @return true if active
     */
    bool CGUIManager::getEventCallerByElement(gui::EGUI_EVENT_TYPE guiEventType) {
        return elementStatus[guiEventType];
    }

    /**
     * This function will be used to get the ID of a gui element.
     * This is a general function for getting the ID.
     * It works for a lot of gui events but not all.
     * Like getting the ID of the context menu wont work with this function
     * Instead, use this function: getEventCallerOfMenuByID()
     * 
     * @return ID of GUI element
     */
    s32 CGUIManager::getEventCallerByID() {
        return generalCallerID;
    }

    /**
     * meant for event: EGET_MENU_ITEM_SELECTED
     * because IGUIContextMenu does not have the function: getID()
     * in this line: event.GUIEvent.Caller->getID()
     * So I had to make a custome one for the EGET_MENU_ITEM_SELECTED events.
     * 
     * @return ID of GUI menu element
     */
    s32 CGUIManager::getEventCallerOfMenuByID() {
        return menuItemSelectedID;
    }
} // end of namespace irr
