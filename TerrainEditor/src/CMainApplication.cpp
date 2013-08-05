/* 
 * File:   CMainApplication.cpp
 * Author: hokan
 * 
 * Created on 26 Июль 2013 г., 13:35
 */

#include "../headers/CMainApplication.h"

namespace irr {

    /**
     * Constructor
     * Sets game data archives, create Irrlicht & application stuff, skydome
     * also creates EventReceiverManager and fills it with class instances
     * 
     * @param videoDriverType       video driver type (OpenGL by default)
     * @param fullScreen            enable full screen mode flag (false by default)
     */
    CMainApplication::CMainApplication(video::E_DRIVER_TYPE videoDriverType, bool fullScreenMode) {
        pDevice = createDevice(videoDriverType, core::dimension2d<u32> (800, 600), 32, fullScreenMode, false, false);
        if (!pDevice)
            exit(0);
        this->fullScreenMode = fullScreenMode;

        pVideoDriver = pDevice->getVideoDriver();
        pSceneManager = pDevice->getSceneManager();

        pDevice->getFileSystem()->addFileArchive("texture/");

        scene::ISceneNode* skydome = pSceneManager->addSkyDomeSceneNode(pVideoDriver->getTexture("Data/textures/skydome.jpg"), 16, 8, 0.95f, 2.0f);
        skydome->setRotation(core::vector3df(0, -100, 0));

        // Data/textures/splatting_test2.png
        pTerrainEditor = new CTerrainEditor(pDevice, "texture/terrain-heightmap4.png", 10.0f);

        // create GUI enviroment & set its font
        pGUIEnviroment = pDevice->getGUIEnvironment();
        gui::IGUIFont* font = pGUIEnviroment->getFont("fonthaettenschweiler.bmp");
        pGUIEnviroment->getSkin()->setFont(font);

        pGUIManager = new CGUIManager(pDevice, pTerrainEditor);

        pEventReceiverManager = new CEventReceiverManager();
        pScreenShotFactory = new CScreenShotFactory(pDevice, "screenshot");
        pEventReceiverManager->addEventReceiver(SEventReceiver(pScreenShotFactory, "pScreenShotFactory", false));
        pEventReceiverManager->addEventReceiver(SEventReceiver(pTerrainEditor, "pTerrainEditor", false));
        pEventReceiverManager->addEventReceiver(SEventReceiver(pTerrainEditor->getBrushManager(), "pBrushManager", false));
        //pEventReceiverManager->addEventReceiver(SEventReceiver(pTerrainEditor->getDecalManager(), "pDecalManager", true));
        pEventReceiverManager->addEventReceiver(SEventReceiver(pGUIManager, "pGUIManager", false));
        pEventReceiverManager->addEventReceiver(SEventReceiver(this, "pMainApplication", false));
        pDevice->setEventReceiver(pEventReceiverManager);
    }

    /**
     * Virtual destructor
     * Clear all created stuff (terrain editor application class instances) & clear-&-drop device
     */
    CMainApplication::~CMainApplication() {
        closeEverything();
        pDevice->closeDevice();
        pDevice->drop();
    }

    /**
     * Clear all created stuff: event receiver manager, GUI manager, screenshot factory, terrain editor classes instances
     */
    void CMainApplication::closeEverything() {
        delete pEventReceiverManager;
        delete pGUIManager;
        delete pScreenShotFactory;
        delete pTerrainEditor;
    }

    /**
     * Operates events correspondes to application itself (exit from application, for example)
     * 
     * @param event incomming event
     * @return true if incomming event processed
     */
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

    /**
     * Run main device loop & run all other drawing function from subclasses
     */
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

    /**
     * Getter
     * 
     * @return current full screen mode flag value
     */
    bool CMainApplication::getFullScreenMode() const {
        return fullScreenMode;
    }

    /**
     * Setter
     * 
     * @param fullScreenMode new boolean flag for full screen mode
     */
    void CMainApplication::setFullScreenMode(bool fullScreenMode) {
        this->fullScreenMode = fullScreenMode;
    }

} // end of namespace irr
