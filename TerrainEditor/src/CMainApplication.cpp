/* 
 * File:   CMainApplication.cpp
 * Author: hokan
 * 
 * Created on 26 Июль 2013 г., 13:35
 */

#include "../headers/CMainApplication.h"

namespace irr {

    CMainApplication::CMainApplication(video::E_DRIVER_TYPE videoDriverType, bool fullScreen) {
        pDevice = createDevice(videoDriverType, core::dimension2d<u32> (800, 600), 32, fullScreen, false, false);
        if (!pDevice)
            exit(0);

        pVideoDriver = pDevice->getVideoDriver();
        pSceneManager = pDevice->getSceneManager();

        pDevice->getFileSystem()->addFileArchive("texture/");

        scene::ISceneNode* skydome = pSceneManager->addSkyDomeSceneNode(pVideoDriver->getTexture("Data/textures/skydome.jpg"), 16, 8, 0.95f, 2.0f);
        skydome->setRotation(core::vector3df(0, -100, 0));

        pTerrainEditor = new CTerrainEditor(pDevice, "Data/textures/splatting_test2.png", 10.0f);

        // create GUI enviroment & set its font
        pGUIEnviroment = pDevice->getGUIEnvironment();
        gui::IGUIFont* font = pGUIEnviroment->getFont("fonthaettenschweiler.bmp");
        pGUIEnviroment->getSkin()->setFont(font);

        pGUIManager = new CGUIManager(pDevice, pTerrainEditor);
        
        pEventReceiverManager = new CEventReceiverManager();
        pScreenShotFactory = new CScreenShotFactory(pDevice, "screen", 0);
        pEventReceiverManager->addEventReceiver(SEventReceiver(pScreenShotFactory, "pScreenShotFactory", false));
        pEventReceiverManager->addEventReceiver(SEventReceiver(pTerrainEditor, "pTerrainEditor", false));
        pEventReceiverManager->addEventReceiver(SEventReceiver(pTerrainEditor->getBrushManager(), "pBrushManager", false));
        pEventReceiverManager->addEventReceiver(SEventReceiver(pGUIManager, "pGUIManager", false));
        pEventReceiverManager->addEventReceiver(SEventReceiver(this, "pMainApplication", false));
        pDevice->setEventReceiver(pEventReceiverManager);
    }

    CMainApplication::CMainApplication(const CMainApplication& orig) {
    }

    CMainApplication::~CMainApplication() {
        closeEverything();
        pDevice->closeDevice();
        pDevice->drop();
    }

    void CMainApplication::closeEverything() {
        delete pEventReceiverManager;
        delete pGUIManager;
        delete pScreenShotFactory;
        delete pTerrainEditor;
    }

    bool CMainApplication::OnEvent(const SEvent& event) {
        if ((event.EventType == EET_KEY_INPUT_EVENT) && event.KeyInput.PressedDown) {
            if (event.KeyInput.Key == KEY_ESCAPE) {
                closeEverything();
                pDevice->closeDevice();
                pDevice->drop();
            }
        }
        return false;
    }

    void CMainApplication::runDevice() {
        while (pDevice->run()) {
            if (pDevice->isWindowActive()) {
                pGUIManager->resetAllGUIElements();
                core::stringw windowCaption = "Terrain Editor FPS = ";
                windowCaption += pVideoDriver->getFPS();
                pDevice->setWindowCaption(windowCaption.c_str());

                pVideoDriver->beginScene(true, true, video::SColor(50, 50, 50, 50));
                pSceneManager->drawAll();
                pTerrainEditor->drawAll();

                //sprintf(c,"elevation step : %.0f units", step);
                //txt[1]->setText(core::stringw(c).c_str());

                //drawCircleBrushBorder(pVideoDriver, pTerrainSceneNode, pTerrainSceneNode->getTerrainCenter(), brushSize, video::SColor(0, 255, 0, 0));
                //drawSquareBrushBorder(pVideoDriver, pTerrainSceneNode, pTerrainSceneNode->getTerrainCenter(), brushSize, yAngle, video::SColor(0, 255, 0, 0));

                pGUIEnviroment->drawAll();

                pVideoDriver->endScene();
            } else {
                pDevice->yield();
            }
        }
    }

} // end of namespace irr
