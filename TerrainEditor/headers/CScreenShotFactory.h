/* 
 * File:   CScreenShotFactory.h
 * Author: hokan
 *
 * Created on 25 Июль 2013 г., 0:04
 */

#ifndef CSCREENSHOTFACTORY_H
#define	CSCREENSHOTFACTORY_H

#include "IEventReceiver.h"
#include <ISceneNode.h>
#include <IrrlichtDevice.h>

namespace irr
{
    class CScreenShotFactory : public IEventReceiver {
    public:
        CScreenShotFactory(IrrlichtDevice *device, const c8* templateName, scene::ISceneNode* node);
        CScreenShotFactory(const CScreenShotFactory& orig) {};
        virtual ~CScreenShotFactory() {};
        virtual bool OnEvent(const SEvent& event);
    private:
        IrrlichtDevice* Device;
        u32 Number;
        core::stringc FilenameTemplate;
        scene::ISceneNode* Node;
    };
} // end of namespace irr       

#endif	/* CSCREENSHOTFACTORY_H */

