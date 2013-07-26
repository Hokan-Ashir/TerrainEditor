/* 
 * File:   CScreenShotFactory.cpp
 * Author: hokan
 * 
 * Created on 25 Июль 2013 г., 0:04
 */

#include "../headers/CScreenShotFactory.h"

namespace irr {

    CScreenShotFactory::CScreenShotFactory(IrrlichtDevice *device, const c8 * templateName, scene::ISceneNode* node)
    : Device(device), Number(0), FilenameTemplate(templateName), Node(node) {
        FilenameTemplate.replace('/', '_');
        FilenameTemplate.replace('\\', '_');
    }

    bool CScreenShotFactory::OnEvent(const SEvent& event) {
        // check if user presses the key print screen
        if ((event.EventType == EET_KEY_INPUT_EVENT) && event.KeyInput.PressedDown) {
            if (event.KeyInput.Key == KEY_F9) {
                video::IImage* image = Device->getVideoDriver()->createScreenShot();
                if (image) {
                    c8 buf[256];
                    snprintf(buf, 256, "%s_shot%04d.jpg",
                            FilenameTemplate.c_str(),
                            ++Number);
                    Device->getVideoDriver()->writeImageToFile(image, buf, 85);
                    image->drop();
                }
            } /*else if (event.KeyInput.Key == KEY_F8) {
                if (Node->isDebugDataVisible())
                    Node->setDebugDataVisible(scene::EDS_OFF);
                else
                    Node->setDebugDataVisible(scene::EDS_BBOX_ALL);
            }*/
        }
        return false;
    }
} // end of namespace irr


