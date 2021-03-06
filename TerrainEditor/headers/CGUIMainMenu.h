#ifndef __C_GUI_MAIN_MENU_H_INCLUDED__
#define __C_GUI_MAIN_MENU_H_INCLUDED__

#include "IGUIEnvironment.h"

namespace irr {

    namespace gui {

        /**
         * Total enumeration for all GUI menu elements
         */
        enum {
            GUI_ID_DIALOG_ROOT_WINDOW = 0x10000,

            GUI_ID_X_SCALE,
            GUI_ID_Y_SCALE,
            GUI_ID_Z_SCALE,

            GUI_ID_OPEN_MODEL,
            GUI_ID_SET_MODEL_ARCHIVE,
            GUI_ID_LOAD_AS_OCTREE,

            GUI_ID_BUTTON_SAVE_SCENE,
            GUI_ID_BUTTON_PAINT,
            GUI_ID_BUTTON_LIFT_DOWN,
            
            GUI_ID_WINDOW_SAVE_HEIGHTMAP,
            GUI_ID_EDITBOX_HEIGHTMAP_NAME,
            GUI_ID_BUTTON_SAVE,
            GUI_ID_BUTTON_CANCEL,

            //GUI_ID_MENU_BRUSHES,
            GUI_ID_GUI_CHECK_CIRCLE_BRUSH,
            GUI_ID_GUI_CHECK_SQUARE_BRUSH,            
            
            GUI_ID_DEBUG_BOUNDING_BOX,
            GUI_ID_DEBUG_NORMALS,
            GUI_ID_DEBUG_SKELETON,
            GUI_ID_DEBUG_WIRE_OVERLAY,
            GUI_ID_DEBUG_HALF_TRANSPARENT,
            GUI_ID_DEBUG_BUFFERS_BOUNDING_BOXES,
            GUI_ID_DEBUG_ALL,

            GUI_ID_MODEL_MATERIAL_SOLID,
            GUI_ID_MODEL_MATERIAL_TRANSPARENT,
            GUI_ID_MODEL_MATERIAL_REFLECTION,

            GUI_ID_CAMERA_MAYA,
            GUI_ID_CAMERA_FIRST_PERSON,

            GUI_ID_POSITION_TEXT,

            GUI_ID_ABOUT,
            GUI_ID_QUIT,

            GUI_ID_TEXTUREFILTER,
            GUI_ID_SKIN_TRANSPARENCY,
            GUI_ID_SKIN_ANIMATION_FPS,

            GUI_ID_BUTTON_SET_SCALE,
            GUI_ID_BUTTON_SCALE_MUL10,
            GUI_ID_BUTTON_SCALE_DIV10,
            GUI_ID_BUTTON_OPEN_MODEL,
            GUI_ID_BUTTON_SHOW_ABOUT,
            GUI_ID_BUTTON_SHOW_TOOLBOX,
            GUI_ID_BUTTON_SELECT_ARCHIVE,

            GUI_ID_ANIMATION_INFO,

            // And some magic numbers
            MAX_FRAMERATE = 80,
            DEFAULT_FRAMERATE = 30
        };

        /**
         * Class simply creates all GUI elements and attach to them constants from enumeration above
         */
        class CGUIMainMenu {
        public:
            /**
             * Constructor
             * 
             * @param pGUIEnviroment pointer to IGUIEnviroment interface
             */
            CGUIMainMenu(IGUIEnvironment* pGUIEnviroment);

            /**
             * Virtual destructor (not implemented)
             */
            virtual ~CGUIMainMenu() {
            };            
        };

    } // end of namespace gui

} // end of namespace irr

#endif	// end of __C_GUI_MAIN_MENU_H_INCLUDED__
