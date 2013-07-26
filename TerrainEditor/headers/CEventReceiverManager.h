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

namespace irr
{    
    struct SEventReceiver
    {
        SEventReceiver(IEventReceiver* pEventReceiver, core::stringw eventReceiverName, bool allowFutherEventsHandle)
        {
            this->pEventReceiver = pEventReceiver;
            this->eventReceiverName = eventReceiverName;
            this->allowFutherEventsHandle = allowFutherEventsHandle;
        }
        IEventReceiver* pEventReceiver;
        core::stringw eventReceiverName;
        bool allowFutherEventsHandle;
    };
        
    class CEventReceiverManager : public IEventReceiver {
    public:
        CEventReceiverManager() {};
        virtual ~CEventReceiverManager() {};
        void addEventReceiver(SEventReceiver eventReceiver);
        void removeEventReceiver(IEventReceiver* pEventReceiver);
        void removeEventReceiverByName(core::stringw eventReceiverName);
        virtual bool OnEvent(const SEvent& event);
        
    private:        
        CEventReceiverManager(const CEventReceiverManager& original);
        CEventReceiverManager& operator=(const CEventReceiverManager&);               
        
        core::array<SEventReceiver> eventReceivers;
    };
} // end of namespace irr

#endif	/* CEVENTRECEIVERMANAGER_H */

