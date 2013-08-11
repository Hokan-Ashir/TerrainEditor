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
                gui::IGUIEnvironment* pGUIEnvironment = pDevice->getGUIEnvironment();
                // create save terrain [height]map window
                gui::IGUIWindow* saveTerrainMapWindow = pGUIEnvironment->addWindow(core::rect<s32>(
                        core::position2d<s32>(pDevice->getVideoDriver()->getScreenSize().Width / 2 - WINDOW_SIZE_WIDTH / 2,
                        pDevice->getVideoDriver()->getScreenSize().Height / 2 - WINDOW_SIZE_HEIGHT / 2),
                        core::position2d<s32>(pDevice->getVideoDriver()->getScreenSize().Width / 2 + WINDOW_SIZE_WIDTH / 2,
                        pDevice->getVideoDriver()->getScreenSize().Height / 2 + WINDOW_SIZE_HEIGHT / 2)),
                        true, L"Save terrain heightmap", 0, gui::GUI_ID_WINDOW_SAVE_HEIGHTMAP);
                //saveTerrainHeightMapWindow->getMaximizeButton()->setVisible(false);
                //saveTerrainHeightMapWindow->getMinimizeButton()->setVisible(false);

                s32 fontHeight = pGUIEnvironment->getSkin()->getFont(gui::EGDF_DEFAULT)->getDimension(L"Enter terrain heightmap file name:").Height;
                s32 xBegin = WINDOW_SIZE_WIDTH / 20;
                s32 xEnd = xBegin + pGUIEnvironment->getSkin()->getFont(gui::EGDF_DEFAULT)->getDimension(L"Enter terrain heightmap file name:").Width;
                s32 yBegin = WINDOW_SIZE_HEIGHT / 8;
                s32 yEnd = WINDOW_SIZE_HEIGHT / 8 + fontHeight;
                pGUIEnvironment->addStaticText(L"Enter terrain heightmap file name:",
                        core::rect<s32>(xBegin,
                        yBegin,
                        xEnd,
                        yEnd),
                        false, false, saveTerrainMapWindow);

                yBegin = yEnd;
                yEnd = yBegin + fontHeight;
                pGUIEnvironment->addEditBox(L"",
                        core::rect<s32>(xBegin,
                        yBegin,
                        xEnd,
                        yEnd),
                        true, saveTerrainMapWindow, gui::GUI_ID_EDITBOX_HEIGHTMAP_NAME);

                yBegin = yEnd + fontHeight;
                xEnd = xBegin + BUTTON_SIZE_WIDTH;
                yEnd = yBegin + BUTTON_SIZE_HEIGHT;
                pGUIEnvironment->addButton(core::rect<s32>(xBegin,
                        yBegin,
                        xEnd,
                        yEnd),
                        saveTerrainMapWindow, gui::GUI_ID_BUTTON_SAVE, L"Save");

                xBegin = xEnd + fontHeight;
                xEnd = xBegin + BUTTON_SIZE_WIDTH;
                pGUIEnvironment->addButton(core::rect<s32>(xBegin,
                        yBegin,
                        xEnd,
                        yEnd),
                        saveTerrainMapWindow, gui::GUI_ID_BUTTON_CANCEL, L"Cancel");
            }
                break;
            case gui::GUI_ID_BUTTON_SAVE:
            {
                // TODO save map. not only heightmap
                const wchar_t* terrainMapName = pDevice->getGUIEnvironment()->getRootGUIElement()->getElementFromId(gui::GUI_ID_EDITBOX_HEIGHTMAP_NAME, true)->getText();
                if (wcscmp(terrainMapName, L"") != 0) {
                    pTerrainEditor->saveTerrainHeightMap(terrainMapName);
                    gui::IGUIElement* saveTerrainMapWindow = pDevice->getGUIEnvironment()->getRootGUIElement()->getElementFromId(gui::GUI_ID_WINDOW_SAVE_HEIGHTMAP, true);
                    // this is a workaround to make sure it's no longer the hovered element, crashes on pressing 1-2 times ESC
                    // From CGUIMessageBox.cpp
                    saveTerrainMapWindow->setVisible(false);
                    pDevice->getGUIEnvironment()->setFocus(0);
                    saveTerrainMapWindow->remove();
                }
            }
                break;
            case gui::GUI_ID_BUTTON_CANCEL:
            {
                gui::IGUIElement* saveTerrainMapWindow = pDevice->getGUIEnvironment()->getRootGUIElement()->getElementFromId(gui::GUI_ID_WINDOW_SAVE_HEIGHTMAP, true);
                // this is a workaround to make sure it's no longer the hovered element, crashes on pressing 1-2 times ESC
                // From CGUIMessageBox.cpp
                saveTerrainMapWindow->setVisible(false);
                pDevice->getGUIEnvironment()->setFocus(0);
                saveTerrainMapWindow->remove();
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
