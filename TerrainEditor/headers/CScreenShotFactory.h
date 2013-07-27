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
    /**
     * Class-factory for screenshot creation, counting its number
     * 
     * inspired by Irrlicht tutorial [16]
     */
    class CScreenShotFactory : public IEventReceiver {
    public:
        /**
         * Constructor
         * 
         * @param pDevice        pointer to IrrlichtDevice
         * @param templateName  file name templates
         */
        CScreenShotFactory(IrrlichtDevice* pDevice, const c8* templateName);
        
        /**
         * Copy constructor (not implemented)
         * 
         * @param orig  reference to original class instance
         */
        CScreenShotFactory(const CScreenShotFactory& orig) {};
        
        /**
         * Virtual destructor (not implemented)
         */
        virtual ~CScreenShotFactory() {};
        
        /**
         * Operates events producing new screenshots (pressing F9 button)
         * 
         * @param event incomming event
         * @return true if incomming event processed
         */
        virtual bool OnEvent(const SEvent& event);
    private:
        /**
         * Pointer to IrrlichtDevice
         */
        IrrlichtDevice* pDevice;
        
        /**
         * Number of previous screenshot
         */
        u32 screenShotNumber;
        
        /**
         * Current screen shot file name template
         */
        core::stringw filenameTemplate;
    };
} // end of namespace irr       

#endif	/* CSCREENSHOTFACTORY_H */

