/* 
 * File:   CMainApplication.h
 * Author: hokan
 *
 * Created on 26 Июль 2013 г., 13:35
 */

#ifndef CMAINAPPLICATION_H
#define	CMAINAPPLICATION_H

#include <IrrlichtDevice.h>
#include <irrlicht.h>

#include "CTerrainEditor.h"
#include "CEventReceiverManager.h"
#include "CScreenShotFactory.h"
#include "CGUIManager.h"

namespace irr {

    /**
     * Class of main application; called from "main" entry point
     * Creates Irrlicht stuff (device, scene manager, video driver, GUI Enviroment etc),
     * other usefull classes (CTerrainEditor, CScreenShotFactory, CGUIManager)
     * control device events & main loop
     */
    class CMainApplication : public IEventReceiver {
    public:
        /**
         * Constructor
         * Sets game data archives, create Irrlicht & application stuff, skydome
         * also creates EventReceiverManager and fills it with class instances
         * 
         * @param videoDriverType       video driver type (OpenGL by default)
         * @param fullScreen            enable full screen mode flag (false by default)
         */
        CMainApplication(video::E_DRIVER_TYPE videoDriverType = video::EDT_OPENGL, bool fullScreenMode = false);
        
        /**
         * Copy constructor (not implemented)
         * 
         * @param orig reference to original class instance
         */
        CMainApplication(const CMainApplication& orig) {};
        
        /**
         * Virtual destructor
         * Clear all created stuff (terrain editor application class instances) & clear-&-drop device
         */
        virtual ~CMainApplication();
        
        /**
         * Run main device loop & run all other drawing function from subclasses
         */
        void runDevice();
        
        /**
         * Operates events correspondes to application itself (exit from application, for example)
         * 
         * @param event incomming event
         * @return true if incomming event processed
         */
        virtual bool OnEvent(const SEvent& event);
        
        /**
         * Clear all created stuff: event receiver manager, GUI manager, screenshot factory, terrain editor classes instances
         */
        void closeEverything();
        
        /**
         * Getter
         * 
         * @return current full screen mode flag value
         */
        bool getFullScreenMode() const;
        
        /**
         * Setter
         * 
         * @param fullScreenMode new boolean flag for full screen mode
         */
        void setFullScreenMode(bool fullScreenMode);                
    private:
        /**
         * Current full screen mode flag value
         */
        bool fullScreenMode;
        
        /**
         * Pointer to IrrlichtDevice class instance
         */
        IrrlichtDevice* pDevice;
        
        /**
         * Pointer to IVideoDriver interface class instance
         */
        video::IVideoDriver* pVideoDriver;
        
         /**
         * Pointer to ISceneManager interface class instance
         */
        scene::ISceneManager* pSceneManager;

        /**
         * Pointer to CEventReceiverManager class instance
         */
        CEventReceiverManager* pEventReceiverManager;
        
        /**
         * Pointer to CScreenShotFactory class instance
         */
        CScreenShotFactory* pScreenShotFactory;
        
        /**
         * Pointer to CTerrainEditor class instance
         */
        CTerrainEditor* pTerrainEditor;

         /**
         * Pointer to IGUIEnvironment interface class instance
         */
        gui::IGUIEnvironment* pGUIEnviroment;
        
        /**
         * Pointer to CGUIManager class instance
         */
        CGUIManager* pGUIManager;
    };

} // end of namespace irr

#endif	/* CMAINAPPLICATION_H */

