/* 
 * File:   CEventReceiverManager.h
 * Author: hokan
 *
 * Created on 24 Июль 2013 г., 22:54
 */

#ifndef CEVENTRECEIVERMANAGER_H
#define	CEVENTRECEIVERMANAGER_H

#include "IEventReceiver.h"
#include <irrArray.h>

namespace irr {

    /**
     * Struct for storing pointers to IEventReceivers, its names and boolean flags
     *  that indicate to process events futher, breaking "chain of responsibility" (CoR) pattern style
     * 
     * inspired by vitek post [http://irrlicht.sourceforge.net/forum//viewtopic.php?t=13568]
     */
    struct SEventReceiver {

        SEventReceiver(IEventReceiver* pEventReceiver, core::stringw eventReceiverName, bool allowFutherEventsHandle) {
            this->pEventReceiver = pEventReceiver;
            this->eventReceiverName = eventReceiverName;
            this->allowFutherEventsHandle = allowFutherEventsHandle;
        }
        IEventReceiver* pEventReceiver;
        core::stringw eventReceiverName;
        bool allowFutherEventsHandle;
    };

    /**
     * Class for primitive control event receivers; just add & remove methods and CoR pattern calling style
     * inside OnEvent method
     * 
     * WARNING
     * this class DO NOT manage event receivers memory, be careful
     */
    class CEventReceiverManager : public IEventReceiver {
    public:

        /**
         * Constructor (not implemented)
         */
        CEventReceiverManager() {
        };

        /**
         * Virtual destructor (not implemented)
         */
        virtual ~CEventReceiverManager() {
        };

        /**
         * Add event receiver, its name and breaking-CoR-allow-flag to array of event receivers
         * 
         * @param eventReceiver struct of event receiver parameters
         * @return true if event receiver has been added
         */
        bool addEventReceiver(SEventReceiver eventReceiver);

        /**
         * Remove event receiver by pointer; uses linear search, so it's really slow, but really it doesn't need to launch often
         * 
         * @param pEventReceiver        pointer to IEventReceiver interface
         */
        void removeEventReceiver(IEventReceiver* pEventReceiver);

        /**
         * Remove event receiver by name; uses linear search, so it's really slow, but really it doesn't need to launch often
         * 
         * @param eventReceiverName     name of event receiver to be removed
         */
        void removeEventReceiverByName(core::stringw eventReceiverName);

        /**
         * Operates common OnEvent events from application
         * 
         * @param event incomming event
         * @return true if incomming event processed (this result will be used by animators, so don't neglect it)
         */
        virtual bool OnEvent(const SEvent& event);

    private:

        /**
         * Copy constructor (not implemented)
         * marked as private for class to become singleton, but not implemented
         * 
         * @param original instance of original class
         */
        CEventReceiverManager(const CEventReceiverManager& original) {
        };

        /**
         * Equality operator (not implemented)
         * marked as private for class to become singleton, but not implemented
         * 
         * @param operand of equality operator
         * @return result of equality operation
         */
        CEventReceiverManager& operator=(const CEventReceiverManager&) {
        };

        /**
         * Array of event receivers, its names and breaking-CoR-allow-flags
         */
        core::array<SEventReceiver> eventReceivers;
    };
} // end of namespace irr

#endif	/* CEVENTRECEIVERMANAGER_H */

