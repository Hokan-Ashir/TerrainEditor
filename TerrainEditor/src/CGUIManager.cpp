/* 
 * File:   CGUIEventManager.cpp
 * Author: hokan
 * 
 * Created on 26 Июль 2013 г., 16:22
 */

#include "../headers/CGUIManager.h"
#include "../headers/terrain.editor/CSceneNodeAnimatorCameraTerrain.h"
#include "../headers/CGUIMainMenu.h"

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
     * Operates events affecting on GUI - pressing buttons, selecting elements
     * 
     * @param event incomming event
     * @return true if incomming event processed
     */
    bool CGUIManager::OnEvent(const SEvent& event) {
        // parsing GUI event
        if (event.EventType == EET_GUI_EVENT) {
            s32 id = event.GUIEvent.Caller->getID();
            gui::IGUIEnvironment* pGUIEnvironment = pDevice->getGUIEnvironment();

            switch (event.GUIEvent.EventType) {
                case gui::EGET_MENU_ITEM_SELECTED:
                    // a menu item was clicked
                    OnMenuItemSelected((gui::IGUIContextMenu*)event.GUIEvent.Caller);
                    break;

                case gui::EGET_BUTTON_CLICKED:
                    // a button was pressed
                    OnButtonPressed(id);
                    break;

                default:
                    break;
            }
        }
        return false;
    }

    void CGUIManager::OnMenuItemSelected(gui::IGUIContextMenu* pGUIContextMenu) {
        s32 id = pGUIContextMenu->getItemCommandId(pGUIContextMenu->getSelectedItem());
        gui::IGUIEnvironment* pGUIEnvironment = pDevice->getGUIEnvironment();

        switch (id) {
            case gui::GUI_ID_GUI_CHECK_CIRCLE_BRUSH: // Brushes -> Use circle brush
                pGUIContextMenu->setItemChecked(pGUIContextMenu->getSelectedItem(),
                        !pGUIContextMenu->isItemChecked(pGUIContextMenu->getSelectedItem()));
                pGUIContextMenu->setItemChecked(pGUIContextMenu->getSelectedItem() + 1,
                        !(pGUIContextMenu->isItemChecked(pGUIContextMenu->getSelectedItem() + 1)));
                pTerrainEditor->getBrushManager()->setBrushType(0);
                break;
            case gui::GUI_ID_GUI_CHECK_SQUARE_BRUSH: // Brushes -> Use square brush
                pGUIContextMenu->setItemChecked(pGUIContextMenu->getSelectedItem(),
                        !pGUIContextMenu->isItemChecked(pGUIContextMenu->getSelectedItem()));
                pGUIContextMenu->setItemChecked(pGUIContextMenu->getSelectedItem() - 1,
                        !(pGUIContextMenu->isItemChecked(pGUIContextMenu->getSelectedItem() - 1)));
                pTerrainEditor->getBrushManager()->setBrushType(1);
                break;
            default:
                break;
        }
    }

    void CGUIManager::OnButtonPressed(s32 id) {
        switch (id) {
            case gui::GUI_ID_BUTTON_SAVE_SCENE:
            {
                pTerrainEditor->getCameraSceneNode()->setInputReceiverEnabled(false);
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
            }
                break;
            case gui::GUI_ID_BUTTON_PAINT:
                // Turn on / off painting - edit mode
                // & turn off lifting-edit mode if active
                pTerrainEditor->setPaintingEditMode(!pTerrainEditor->getPaintingEditMode());
                if (pTerrainEditor->getLiftingEditMode()) {
                    pTerrainEditor->setLiftingEditMode(!pTerrainEditor->getLiftingEditMode());
                }
                break;
            case gui::GUI_ID_BUTTON_LIFT_DOWN:
                // Turn on/off lifting-edit mode
                // & turn off painting-edit mode if active
                pTerrainEditor->setLiftingEditMode(!pTerrainEditor->getLiftingEditMode());
                if (pTerrainEditor->getPaintingEditMode()) {
                    pTerrainEditor->setPaintingEditMode(!pTerrainEditor->getPaintingEditMode());
                }
                break;
            default:
                break;
        }
    }
} // end of namespace irr
