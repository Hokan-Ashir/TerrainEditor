#include "../headers/CGUIMainMenu.h"
#include "IGUIContextMenu.h"
#include "IVideoDriver.h"
#include "IGUIToolbar.h"
#include "IGUIComboBox.h"

namespace irr {

    namespace gui {

        /**
         * Constructor
         * 
         * @param pGUIEnviroment pointer to IGUIEnviroment interface
         */
        CGUIMainMenu::CGUIMainMenu(IGUIEnvironment* pGUIEnviroment) {
            // create menu
            IGUIContextMenu* pContextMenu = pGUIEnviroment->addMenu();
            /*menu->addItem(L"File", -1, true, true);
            menu->addItem(L"World", -1, true, true);
            menu->addItem(L"Landscape", -1, true, true);
            menu->addItem(L"Objects", -1, true, true);
            menu->addItem(L"Triggers", -1, true, true);*/
            pContextMenu->addItem(L"Brushes", -1, true, true);
            pContextMenu->addItem(L"Help", -1, true, true);

            IGUIContextMenu* submenu;

            // File submenu
            //submenu = menu->getSubMenu(0);
            //submenu->addItem(L"Open scene...", GUI_ID_OPEN_MODEL);
            //submenu->addItem(L"Set Model Archive...", GUI_ID_SET_MODEL_ARCHIVE);
            //submenu->addItem(L"Save scene...", GUI_ID_LOAD_AS_OCTREE);
            //submenu->addSeparator();
            //submenu->addItem(L"Quit", GUI_ID_QUIT);

            // World submenu
            //submenu = menu->getSubMenu(1);
           // submenu->addItem(L"Lightning", GUI_ID_DEBUG_OFF);
            //submenu->addItem(L"Map size", GUI_ID_DEBUG_BOUNDING_BOX);
            /*submenu->addItem(L"Normals", GUI_ID_DEBUG_NORMALS);
            submenu->addItem(L"Skeleton", GUI_ID_DEBUG_SKELETON);
            submenu->addItem(L"Wire overlay", GUI_ID_DEBUG_WIRE_OVERLAY);
            submenu->addItem(L"Half-Transparent", GUI_ID_DEBUG_HALF_TRANSPARENT);
            submenu->addItem(L"Buffers bounding boxes", GUI_ID_DEBUG_BUFFERS_BOUNDING_BOXES);
            submenu->addItem(L"All", GUI_ID_DEBUG_ALL);*/

            // Landscape submenu
            /*submenu = menu->getSubMenu(2);
            submenu->addItem(L"Paint terrain texture", GUI_ID_SKY_BOX_VISIBLE, true, false, true);
            submenu->addItem(L"Paint water texture", GUI_ID_TOGGLE_DEBUG_INFO, true, true);
            submenu->addItem(L"Place tree", GUI_ID_SKY_BOX_VISIBLE, true, false, true);
            submenu->addItem(L"model material", -1, true, true);*/

            // mode material submenu
            /*submenu = menu->getSubMenu(2)->getSubMenu(3);
            submenu->addItem(L"Solid", GUI_ID_MODEL_MATERIAL_SOLID);
            submenu->addItem(L"Transparent", GUI_ID_MODEL_MATERIAL_TRANSPARENT);
            submenu->addItem(L"Reflection", GUI_ID_MODEL_MATERIAL_REFLECTION);*/

            // Objects submenu
            /*submenu = menu->getSubMenu(3);
            submenu->addItem(L"Place object", GUI_ID_CAMERA_MAYA);
            submenu->addItem(L"Delete object(s)", GUI_ID_CAMERA_FIRST_PERSON);*/

            // Triggers submenu
            /*submenu = menu->getSubMenu(4);
            submenu->addItem(L"Create trigger", GUI_ID_CAMERA_MAYA);
            submenu->addItem(L"Delete object(s)", GUI_ID_CAMERA_FIRST_PERSON);*/

            // Brushes submenu
            submenu = pContextMenu->getSubMenu(/*5*/0);
            submenu->addItem(L"Use circle brush", GUI_ID_GUI_CHECK_CIRCLE_BRUSH, true, false, true);
            submenu->addItem(L"Use square brush", GUI_ID_GUI_CHECK_SQUARE_BRUSH);
            
            // Help submenu
            submenu = pContextMenu->getSubMenu(/*5*/1);
            submenu->addItem(L"About", GUI_ID_ABOUT);
            
            

            // create toolbar

            video::IVideoDriver* pVideoDriver = pGUIEnviroment->getVideoDriver();

            IGUIToolBar* bar = pGUIEnviroment->addToolBar();

            //video::ITexture* image = pVideoDriver->getTexture("new.png");
            //bar->addButton(GUI_ID_BUTTON_OPEN_MODEL, 0, L"New scene", image, 0, false, true);

            //image = pVideoDriver->getTexture("open.png");
            //bar->addButton(GUI_ID_BUTTON_SHOW_TOOLBOX, 0, L"Open scene", image, 0, false, true);

            video::ITexture* image = pVideoDriver->getTexture("save.png");
            bar->addButton(GUI_ID_BUTTON_SAVE_SCENE, 0, L"Save", image, 0, false, true);         

            image = pVideoDriver->getTexture("paint.png");
            bar->addButton(GUI_ID_BUTTON_PAINT, 0, L"Paint landscape with selected brush", image, 0, false, true);
            
            image = pVideoDriver->getTexture("lift.png");
            bar->addButton(GUI_ID_BUTTON_LIFT_DOWN, 0, L"Lift/low landscape", image, 0, false, true);

            // create a combobox for texture filters

            /*IGUIComboBox* box = pGUIEnviroment->addComboBox(core::rect<s32>(250, 4, 350, 23), bar, GUI_ID_TEXTUREFILTER);
            box->addItem(L"No filtering");
            box->addItem(L"Bilinear");
            box->addItem(L"Trilinear");
            box->addItem(L"Anisotropic");
            box->addItem(L"Isotropic");*/

            // disable alpha

            for (s32 i = 0; i < EGDC_COUNT; ++i) {
                video::SColor col = pGUIEnviroment->getSkin()->getColor((gui::EGUI_DEFAULT_COLOR)i);
                col.setAlpha(255);
                pGUIEnviroment->getSkin()->setColor((gui::EGUI_DEFAULT_COLOR)i, col);
            }
        }
    } // end of namespace gui

} // end of namespace irr
