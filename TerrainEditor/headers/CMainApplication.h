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

    class CMainApplication : public IEventReceiver {
    public:
        CMainApplication(video::E_DRIVER_TYPE videoDriverType, bool fullScreen);
        CMainApplication(const CMainApplication& orig);
        virtual ~CMainApplication();
        void runDevice();
        virtual bool OnEvent(const SEvent& event);
        void closeEverything();
    private:
        bool fullScreen;
        IrrlichtDevice* pDevice;
        video::IVideoDriver* pVideoDriver;
        scene::ISceneManager* pSceneManager;
        
        CEventReceiverManager* pEventReceiverManager;
        CScreenShotFactory* pScreenShotFactory;        
        CTerrainEditor* pTerrainEditor;
        
        gui::IGUIEnvironment* pGUIEnviroment;
        CGUIManager* pGUIManager;
    };
    
}       // end of namespace irr

#endif	/* CMAINAPPLICATION_H */

